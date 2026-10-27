#include "OrderBook.h"
#include "def.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class ScenarioTest : public testing::TestWithParam<std::string> {
protected:
  OrderBook book;
  std::vector<Trade> recorded_trades;

  void ExecuteLine(const std::string &line, size_t line_no) {
    std::istringstream iss(line);
    std::string action;
    iss >> action;

    if (action == "ADD") {
      std::string side_str;
      Price price;
      Quantity qty;
      iss >> side_str >> price >> qty;

      Side side = (side_str == "BUY") ? Side::buy : Side::sell;
      book.AddOrMatchOrder(Order(side, qty, price));
      auto trades = book.getTrades();
      recorded_trades.insert(recorded_trades.end(), trades.begin(),
                             trades.end());

    } else if (action == "CANCEL") {
      OrderID id;
      iss >> id;
      book.CancelOrder(id);

    } else if (action == "EXPECT") {
      std::string sub;
      iss >> sub;

      if (sub == "TRADE") {
        OrderID passive_id, agg_id;
        Price price;
        Quantity qty;
        iss >> agg_id >> passive_id >> price >> qty;

        ASSERT_FALSE(recorded_trades.empty())
            << "Line " << line_no << ": Expected trade but no trade occurred.";

        Trade actual = recorded_trades.front();
        recorded_trades.erase(recorded_trades.begin());

        EXPECT_EQ(actual.pas_, passive_id)
            << "Line " << line_no << ": Passive ID mismatch";
        EXPECT_EQ(actual.agg_, agg_id)
            << "Line " << line_no << ": Agg ID mismatch";
        EXPECT_EQ(actual.price_, price)
            << "Line " << line_no << ": Price mismatch";
        EXPECT_EQ(actual.quantity_, qty)
            << "Line " << line_no << ": Qty mismatch";

      } else if (sub == "BOOK") {
        std::string side_str;
        iss >> side_str;
        Side side = (side_str == "BID") ? Side::buy : Side::sell;

        std::string token;
        iss >> token;
        if (token == "EMPTY") {
          EXPECT_EQ(book.depth(side), 0)
              << "Line " << line_no << ": Expected empty " << side_str
              << " book.";
        } else {
          Price price = std::stoll(token);
          Quantity qty;
          iss >> qty;
          EXPECT_EQ(book.volume_at_level(side, price), qty)
              << "Line " << line_no << ": Volume mismatch at " << side_str
              << " " << price;
        }
      }
    }
  }
};

TEST_P(ScenarioTest, EvaluatesFile) {
  std::string file_path = GetParam();
  std::ifstream file(file_path);
  ASSERT_TRUE(file.is_open()) << "Failed to open file: " << file_path;

  std::string line;
  size_t line_no = 0;
  while (std::getline(file, line)) {
    ++line_no;
    if (line.empty() || line[0] == '#')
      continue;
    ExecuteLine(line, line_no);
  }

  EXPECT_TRUE(recorded_trades.empty())
      << "Unchecked trades remained at end of test: " << file_path;
}

// Automatically scans tests/scenarios directory for all .scenario files
std::vector<std::string> GetScenarioFiles() {
  std::vector<std::string> files;
  std::string dir = "../tests/scenarios";
  if (fs::exists(dir)) {
    for (const auto &entry : fs::directory_iterator(dir)) {
      if (entry.path().extension() == ".scenario") {
        files.push_back(entry.path().string());
      }
    }
  }
  return files;
}

INSTANTIATE_TEST_SUITE_P(FileDrivenEngineTests, ScenarioTest,
                         testing::ValuesIn(GetScenarioFiles()),
                         [](const testing::TestParamInfo<std::string> &info) {
                           // Formats the test name to the base filename
                           fs::path p(info.param);
                           std::string stem = p.stem().string();
                           std::replace(stem.begin(), stem.end(), '-', '_');
                           return stem;
                         });

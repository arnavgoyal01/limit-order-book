#include "OrderBook.h"
#include "def.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  OrderBook ob;
  std::string input;
  std::string word;
  std::vector<std::string> tokens;
  std::stringstream ss;
  while (true) {
    ss.clear();
    tokens.clear();
    input.clear();
    std::cout << ">> ";
    std::getline(std::cin, input);
    if (input == "quit") {
      break;
    } else if (input == "") {
      continue;
    }
    ss << input;
    while (ss >> word) {
      tokens.push_back(word);
    }

    Side side{};
    Quantity quantity{};
    Price price{};

    if (tokens.at(0) == "buy") {
      side = Side::buy;
    } else if (tokens.at(0) == "sell") {
      side = Side::sell;
    } else if (tokens.at(0) == "cancel") {
      ob.CancelOrder(std::stoi(tokens.at(1)));
      continue;
    } else {
      std::cerr << "Invalid order side\n";
      return 1;
    }
    quantity = std::stoi(tokens.at(1));
    price = std::stoi(tokens.at(2)) * 100;
    Order op = Order(side, quantity, price);
    ob.AddOrMatchOrder(op);
  }

  return 0;
}

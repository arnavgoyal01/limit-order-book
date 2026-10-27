#include "OrderBook.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

OrderBook::OrderBook() {}

// auto OrderBook::GetBestBidLevel() const { return bestBidLevel_; }
// auto OrderBook::GetBestAskLevel() const { return bestAskLevel_; }

void OrderBook::processOrder(Order *op, Level *target) {
  OrderID top;
  while (op->quantity > 0) {
    if (target->isEmpty()) {
      return;
    }
    top = target->peekOrder();
    auto it = orderbook.find(top);
    if (it != orderbook.end()) {
      auto fill_qty = std::min(it->second.quantity, op->quantity);
      it->second.quantity -= fill_qty;
      op->quantity -= fill_qty;
      target->quantity_ -= fill_qty;
      Trade tp(top, op->id, it->second.price, fill_qty);
      // std::cout << "trade created: \n"
      //           << tp.time << " " << top << " " << op->id << " " << op->price
      //           << " " << fill_qty << "\n";
      trades.push_back(std::move(tp));
      if (it->second.quantity == 0) {
        target->popOrder();
      }
    } else {
      target->popOrder();
    }
  }
}

void OrderBook::AddBuyOrder(Order *op) {
  orderbook.insert(std::pair<OrderID, Order>(op->id, *op));
  if (currentBuyOrders.find(op->price) != currentBuyOrders.end()) {
    currentBuyOrders.at(op->price).addOrder(op->id, op->quantity);
  } else {
    currentBuyOrders.insert(std::pair<Price, Level>(op->price, Level(*op)));
  }
}

void OrderBook::AddSellOrder(Order *op) {
  orderbook.insert(std::pair<OrderID, Order>(op->id, *op));
  if (currentSellOrders.find(op->price) != currentSellOrders.end()) {
    currentSellOrders.at(op->price).addOrder(op->id, op->quantity);
  } else {
    currentSellOrders.insert(std::pair<Price, Level>(op->price, Level(*op)));
  }
}

void OrderBook::processBuyOrder(Order *op) {
  if (currentSellOrders.empty() ||
      op->price < currentSellOrders.begin()->second.getPrice()) {
    AddBuyOrder(op);
    return;
  }
  processOrder(
      op,
      &currentSellOrders.begin()->second); // returns when bestAskLevel_ empty
                                           // or op->quantity 0 (order filled)

  if (currentSellOrders.begin()->second.isEmpty()) {
    currentSellOrders.erase(currentSellOrders.begin());
  }
  if (op->quantity > 0) {
    if (currentSellOrders.empty()) {
      AddBuyOrder(op);
      return;
    } else {
      processBuyOrder(op);
    }
  }
}

void OrderBook::processSellOrder(Order *op) {
  if (currentBuyOrders.empty() ||
      op->price > currentBuyOrders.begin()->second.getPrice()) {
    AddSellOrder(op);
    return;
  }
  processOrder(
      op,
      &currentBuyOrders.begin()->second); // returns when bestAskLevel_ empty
                                          // or op->quantity 0 (order filled)

  if (currentBuyOrders.begin()->second.isEmpty()) {
    currentBuyOrders.erase(currentBuyOrders.begin());
  }

  if (op->quantity > 0) {
    if (currentBuyOrders.empty()) {
      AddSellOrder(op);
      return;
    } else {
      processSellOrder(op);
    }
  }
}
void OrderBook::AddOrMatchOrder(Order op) {
  // std::cout << "Order: \n" << op << " CREATED\n";
  switch (op.side) {
  case Side::buy:
    processBuyOrder(&op);
    break;
  case Side::sell:
    processSellOrder(&op);
    break;
  }
}

void OrderBook::CancelOrder(OrderID orderID) {
  if (orderbook.find(orderID) != orderbook.end()) {
    if (orderbook.at(orderID).quantity == 0) {
      std::cout << orderID << " already filled\n";
    } else {
      orderbook.erase(orderID);
      std::cout << orderID << " cancelled\n";
    }
  } else {
    std::cout << orderID << " already cancelled\n";
  }
}

std::vector<Trade> OrderBook::getTrades() const { return trades; }

size_t OrderBook::depth(Side side) const {
  switch (side) {
  case Side::buy:
    return currentBuyOrders.size();
    break;
  case Side::sell:
    return currentSellOrders.size();
    break;
  }
}

Quantity OrderBook::volume_at_level(Side side, Price price) const {
  try {
    switch (side) {
    case Side::buy:
      return currentBuyOrders.at(price).quantity_;
      break;
    case Side::sell:
      return currentSellOrders.at(price).quantity_;
      break;
    }
  } catch (const std::out_of_range &e) {
    return 0;
  }
}

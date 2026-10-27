#pragma once
#include "def.h"
#include <cstddef>
#include <memory>
#include <vector>

class OrderBook {
public:
  OrderBook();
  // auto GetBestBidLevel() const;
  // auto GetBestAskLevel() const;
  void processOrder(Order *op, Level *target);
  void AddOrMatchOrder(Order op);
  void CancelOrder(OrderID orderID);
  void AddBuyOrder(Order *op);
  void AddSellOrder(Order *op);
  void processBuyOrder(Order *op);
  void processSellOrder(Order *op);
  std::vector<Trade> getTrades() const;
  size_t depth(Side side) const;
  Quantity volume_at_level(Side side, Price price) const;

private:
  Book orderbook;
  Bids currentBuyOrders;
  Asks currentSellOrders;
  std::vector<Trade> trades;
};

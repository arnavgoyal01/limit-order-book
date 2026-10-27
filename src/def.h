#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <set>
#include <stdexcept>

enum Side {
  buy,
  sell,
};

enum OrderType {
  market,
  limit,
};

using Quantity = std::uint32_t;
using Price = std::int32_t;
using OrderID = std::uint64_t;
using TimeStamp = std::uint64_t;

uint64_t get_current_time_ms();

static OrderID start{1};

struct Order {
  OrderID id;
  OrderType type;
  Side side;
  Quantity quantity;
  Price price;
  TimeStamp time;

  Order(Side s, Quantity q, Price p, OrderType t = OrderType::limit)
      : type(t), side(s), quantity(q), price(p), time(get_current_time_ms()) {
    id = start;
    start += 1;
  }
  friend std::ostream &operator<<(std::ostream &os, const Order &op);
};

struct Level {
  Price price_;
  Quantity quantity_;
  std::queue<OrderID> orderList_;
  Level(Order op)
      : price_(op.price), orderList_({op.id}), quantity_(op.quantity) {}
  void addOrder(OrderID op, Quantity q);
  OrderID peekOrder();
  void popOrder();
  bool isEmpty() const;
  Price getPrice() const;
};

using Book = std::map<OrderID, Order>;
using Bids = std::map<Price, Level, std::greater<Price>>;
using Asks = std::map<Price, Level, std::less<Price>>;

struct Trade {
  OrderID pas_, agg_;
  Price price_;
  Quantity quantity_;
  TimeStamp time;
  Trade(OrderID passive, OrderID agg, Price p, Quantity q)
      : pas_(passive), agg_(agg), price_(p), quantity_(q),
        time(get_current_time_ms()) {}
};

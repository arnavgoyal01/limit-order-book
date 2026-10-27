#include "def.h"
#include <ostream>
#include <string>

std::ostream &operator<<(std::ostream &os, const Order &op) {
  std::string t;
  switch (op.type) {
  case OrderType::market:
    t = "market";
    break;
  case OrderType::limit:
    t = "limit";
    break;
  }

  std::string s;
  switch (op.side) {
  case Side::buy:
    s = "buy";
    break;
  case Side::sell:
    s = "sell";
    break;
  }
  os << op.time << " " << op.id << " " << t << " " << s << " " << op.quantity
     << " " << op.price;
  return os;
}

uint64_t get_current_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

OrderID Level::peekOrder() {
  auto op = orderList_.front();
  return op;
}

void Level::popOrder() { orderList_.pop(); }

bool Level::isEmpty() const { return orderList_.empty(); }

Price Level::getPrice() const { return price_; }

void Level::addOrder(OrderID op, Quantity q) {
  orderList_.push(op);
  quantity_ += q;
}

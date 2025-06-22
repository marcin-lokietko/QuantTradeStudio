#include "Orders.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ApiGateway::SingleOrder& singleOrder) {
  nlohmann::json j;
  j = nlohmann::json{{"assetPair", toString(singleOrder.assetPair)},
                     {"orderId", singleOrder.orderId.val_},
                     {"price", singleOrder.price.val_},
                     {"origQuantity", singleOrder.origQuantity.val_},
                     {"executedQuantity", singleOrder.executedQuantity.val_},
                     {"orderSide", toString(singleOrder.side)}};
  return j;
}
}  // namespace

nlohmann::json toJson(const ApiGateway::Orders& orders) {
  nlohmann::json j = nlohmann::json::array();
  for (const auto& singleOrder : orders) {
    j.push_back(toJson(singleOrder));
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion
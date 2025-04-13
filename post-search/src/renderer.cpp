#include "renderer.hpp"

#include <userver/formats/json.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/assert.hpp>

namespace post_search {

Renderer::Renderer(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context)
    : userver::ugrpc::server::ServiceComponentBase(config, context),
      server_(context.FindComponent<userver::ugrpc::server::ServerComponent>()) {
    RegisterIn(server_);
}

void Renderer::RegisterIn(userver::components::ComponentList& component_list) {
    component_list.RegisterComponent(*this);
}

grpc::Status Renderer::ConvertToJson(
    grpc::ServerContext* context,
    const InputMessage* request,
    JsonResponse* response) {

    try {
        userver::formats::json::ValueBuilder result_builder;

        if (request->has_result()) {
            const auto& result = request->result();
            userver::formats::json::ValueBuilder result_json;

            result_json["total_items"] = result.total_items();

            if (result.items_size() > 0) {
                userver::formats::json::ValueBuilder items_array =
                    userver::formats::json::ValueBuilder::Array();

                for (const auto& item : result.items()) {
                    userver::formats::json::ValueBuilder item_json;

                    item_json["id"] = item.id();
                    item_json["title"] = item.title();
                    item_json["description"] = item.description();

                    if (item.has_rating()) {
                        item_json["rating"]["value"] = item.rating();
                        item_json["rating"]["stars"] = std::string(
                            std::min(5, static_cast<int>(std::round(item.rating()))), '★');
                    }

                    items_array.PushBack(item_json.ExtractValue());
                }

                result_json["items"] = items_array.ExtractValue();
            }

            result_builder["search_results"] = result_json.ExtractValue();
        }

        if (request->has_category()) {
            const auto& category = request->category();
            userver::formats::json::ValueBuilder category_json;

            category_json["id"] = category.id();
            category_json["name"] = category.name();
            category_json["path"] = category.path();

            if (category.has_parent()) {
                category_json["parent"]["id"] = category.parent().id();
                category_json["parent"]["name"] = category.parent().name();
            }

            result_builder["category"] = category_json.ExtractValue();
        }

        if (request->price_size() > 0) {
            userver::formats::json::ValueBuilder prices_array =
                userver::formats::json::ValueBuilder::Array();

            for (const auto& price : request->price()) {
                userver::formats::json::ValueBuilder price_json;

                price_json["amount"] = price.amount();
                price_json["currency"] = price.currency();

                if (price.has_discount() && price.discount() > 0) {
                    price_json["original_amount"] = price.amount();
                    price_json["discount_percent"] = price.discount() * 100;
                    price_json["discounted_amount"] =
                        price.amount() * (1.0 - price.discount());
                }

                prices_array.PushBack(price_json.ExtractValue());
            }

            result_builder["prices"] = prices_array.ExtractValue();
        }

        if (request->favourite_category_size() > 0) {
            userver::formats::json::ValueBuilder fav_categories_array =
                userver::formats::json::ValueBuilder::Array();

            for (const auto& fav_category : request->favourite_category()) {
                userver::formats::json::ValueBuilder fav_category_json;

                fav_category_json["id"] = fav_category.id();
                fav_category_json["name"] = fav_category.name();

                if (fav_category.has_preference_score()) {
                    fav_category_json["preference_level"] =
                        fav_category.preference_score() > 0.8 ? "high" :
                        fav_category.preference_score() > 0.5 ? "medium" : "low";
                }
                if (fav_category.has_old()) {
                    fav_category_json["is_old"] = fav_category.old();
                    fav_category_json["old_price"] = fav_category.old_price();
                    fav_category_json["old_discount"] = fav_category.old_discount();
                    fav_category_json["old_price_amount"] = fav_category.old_price_amount();
                    if (int accounts = fav_category.accounts_size() > 0; accounts > 0) {
                        userver::formats::json::ValueBuilder accounts_array =
                            userver::formats::json::ValueBuilder::Array();
                        for (const auto& account : fav_category.accounts()) {
                            userver::formats::json::ValueBuilder account_json;
                            account_json["id"] = account.id();
                            accounts_array.PushBack(account_json.ExtractValue());
                        }
                    }
                }

                fav_categories_array.PushBack(fav_category_json.ExtractValue());
            }

            result_builder["favourite_categories"] = fav_categories_array.ExtractValue();
        }

        if (request->has_user()) {
            const auto& user = request->user();
            userver::formats::json::ValueBuilder user_json;

            user_json["id"] = user.id();

            if (user.has_email()) {
                std::string email = user.email();
                size_t at_pos = email.find('@');
                if (at_pos != std::string::npos && at_pos > 1) {
                    email = email.substr(0, 2) + "***" + email.substr(at_pos);
                }
                user_json["email"] = email;
            }

            if (user.has_preferences()) {
                user_json["has_personalization"] = true;
            }

            result_builder["user"] = user_json.ExtractValue();
        }

        response->set_json_data(userver::formats::json::ToString(result_builder.ExtractValue()));
        response->set_success(true);

    } catch (const std::exception& ex) {
        LOG_ERROR() << "Error processing search result: " << ex.what();
        response->set_success(false);
        response->set_error("Failed to convert proto to JSON: " + std::string(ex.what()));
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal conversion error");
    }

    return grpc::Status::OK;
}

}  // namespace post_search

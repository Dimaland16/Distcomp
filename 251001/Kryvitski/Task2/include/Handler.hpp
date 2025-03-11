#pragma once
#include <httplib.h>
#include <concepts>
#include "DBController.hpp"
    
template<Entity T>
class Handler {
public:
    explicit Handler(std::shared_ptr<DBController> controller);
    ~Handler() = default;

    void initialize();
    void handle_post(const httplib::Request& req, httplib::Response& res);
    void handle_get_all(const httplib::Request& req, httplib::Response& res);
    void handle_get_one(const httplib::Request& req, httplib::Response& res, uint64_t id);
    void handle_delete(const httplib::Request& req, httplib::Response& res, uint64_t id);
    void handle_put(const httplib::Request& req, httplib::Response& res);

private:
    std::shared_ptr<DBController> m_controller;
};

template <Entity T>
inline Handler<T>::Handler(std::shared_ptr<DBController> controller)
{
    m_controller = std::move(controller);
}

template <Entity T>
void Handler<T>::initialize()
{
    auto result = m_controller->create_table<T>();
}

template<Entity T>
void Handler<T>::handle_post(const httplib::Request& req, httplib::Response& res) {
    T entity{};
    try {
        entity = T::from_json(req.body);
    }
    catch (const std::exception& e){
        res.status = 400;
        res.set_content(json{{"error", "Invalid JSON"}}.dump(), "application/json");
        return;
    }

    if (m_controller->insert(entity)) {
        res.status = 201;
        res.set_content(entity.to_json().dump(), "application/json");
    } else {
        res.status = 403;
        res.set_content(json{{"error", "Insert failure"}}.dump(), "application/json");
    }
}

template<Entity T>
void Handler<T>::handle_get_all(const httplib::Request& req, httplib::Response& res) {
    std::vector<T> entities = m_controller->get_all<T>();

    json entities_json = json::array();
    std::ranges::transform(entities, std::back_inserter(entities_json), 
        [this](const T& entity) 
    {
        return entity.to_json();
    });

    res.status = 200;
    res.set_content(entities_json.dump(), "application/json");
}

template<Entity T>
void Handler<T>::handle_get_one(const httplib::Request& req, httplib::Response& res, uint64_t id) {
    std::optional<T> entity = m_controller->get_by_id<T>(id);

    if (entity.has_value()) {
        res.status = 200;
        res.set_content(entity.value().to_json().dump(), "application/json");
    }
    else {
        res.status = 404;
        res.set_content(json{{"error", std::format("Id not found", id)}}.dump(), "application/json");
    }
}

template<Entity T>
void Handler<T>::handle_delete(const httplib::Request& req, httplib::Response& res, uint64_t id) {
    if (m_controller->delete_by_id<T>(id)){
        res.status = 204;
    }
    else{
        res.status = 404;
    }
}

template<Entity T>
void Handler<T>::handle_put(const httplib::Request& req, httplib::Response& res) {
    T entity = T::from_json(req.body);
    m_controller->update_by_id(entity);
    res.status = 200;
    res.set_content(entity.to_json().dump(), "application/json");
}

#include <control_plane.h>


std::shared_ptr<pdn_connection> control_plane::find_pdn_by_cp_teid(uint32_t cp_teid) const {
    auto it = _pdns.find(cp_teid);
    if (it != _pdns.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<pdn_connection> control_plane::find_pdn_by_ip_address(const boost::asio::ip::address_v4 &ip) const {
    auto it = _pdns_by_ue_ip_addr.find(ip);
    if (it != _pdns_by_ue_ip_addr.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<bearer> control_plane::find_bearer_by_dp_teid(uint32_t dp_teid) const {
    auto it = _bearers.find(dp_teid);
    if (it != _bearers.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<pdn_connection> control_plane::create_pdn_connection(
    const std::string &apn,
    boost::asio::ip::address_v4 sgw_addr,
    uint32_t sgw_cp_teid)
{
    // ищем APN по имени
    auto apn_it = _apns.find(apn);
    if (apn_it == _apns.end()) {
        throw std::runtime_error("Unknown APN: " + apn);
    }

    static uint32_t next_cp_teid = 1;
    static uint32_t next_ue_ip_suffix = 1;

    // генерируем TEID и IP для UE
    uint32_t cp_teid = next_cp_teid++;
    auto ue_ip = boost::asio::ip::make_address_v4("10.0.0." + std::to_string(next_ue_ip_suffix++));

    // создаем соединение
    auto pdn = pdn_connection::create(cp_teid, apn_it->second, ue_ip);
    pdn->set_sgw_cp_teid(sgw_cp_teid);
    pdn->set_sgw_addr(sgw_addr);

    // сохраняем
    _pdns[cp_teid] = pdn;
    _pdns_by_ue_ip_addr[ue_ip] = pdn;

    return pdn;
}

void control_plane::delete_pdn_connection(uint32_t cp_teid) {
    // ищем
    auto it = _pdns.find(cp_teid);
    if (it != _pdns.end()) {
        auto pdn = it->second;

        // удаляем все bearers этой PDN
        for (const auto &[teid, b] : pdn->_bearers) {
            _bearers.erase(teid);
        }

        _pdns_by_ue_ip_addr.erase(pdn->get_ue_ip_addr());
        _pdns.erase(it);
    }
}

std::shared_ptr<bearer> control_plane::create_bearer(const std::shared_ptr<pdn_connection> &pdn, uint32_t sgw_teid) {
    // генерируем teid
    uint32_t dp_teid = _next_dp_teid++;
    // создаем новый bearer
    auto b = std::make_shared<bearer>(dp_teid, *pdn);
    b->set_sgw_dp_teid(sgw_teid);

    // сохраняем
    pdn->add_bearer(b);
    _bearers[dp_teid] = b;

    return b;
}

void control_plane::delete_bearer(uint32_t dp_teid) {
    // ищем
    auto it = _bearers.find(dp_teid);
    // удаляем
    if (it != _bearers.end()) {
        auto b = it->second;
        auto pdn = b->get_pdn_connection();
        if (pdn) {
            pdn->remove_bearer(dp_teid);
        }
        _bearers.erase(it);
    }
}

void control_plane::add_apn(std::string apn_name, boost::asio::ip::address_v4 apn_gateway) {
    _apns[std::move(apn_name)] = apn_gateway;
}
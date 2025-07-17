#include <data_plane.h>

data_plane::data_plane(control_plane &control_plane, uint64_t uplink_bps, uint64_t downlink_bps) : 
     _control_plane(control_plane),
        _uplink_bucket(uplink_bps, uplink_bps),
        _downlink_bucket(downlink_bps, downlink_bps) {}

void data_plane::handle_uplink(uint32_t dp_teid, Packet &&packet) {
    if (!_uplink_bucket.allow(packet.size())) {
        std::cerr << "Uplink limited, dropping packet\n";
        return;
    }
    // ищем bearer по TEID
    auto bearer = _control_plane.find_bearer_by_dp_teid(dp_teid);
    if (!bearer) return;

    auto pdn = bearer->get_pdn_connection();
    if (!pdn) return;

    // получаем адрес APN Gateway
    auto apn_gateway = pdn->get_apn_gw();

    // передаём пакет в APN
    forward_packet_to_apn(apn_gateway, std::move(packet));
}

void data_plane::handle_downlink(const boost::asio::ip::address_v4 &ue_ip, Packet &&packet) {
    if (!_downlink_bucket.allow(packet.size())) {
        std::cerr << "Downlink limited, dropping packet\n";
        return;
    }
    // находим PDN по IP
    auto pdn = _control_plane.find_pdn_by_ip_address(ue_ip);
    if (!pdn) return;

    auto bearer = pdn->get_default_bearer();
    if (!bearer) return;

    // получаем адрес SGW и TEID на стороне SGW
    auto sgw_addr = pdn->get_sgw_address();
    auto sgw_teid = bearer->get_sgw_dp_teid();

    // передаём пакет в SGW 
    forward_packet_to_sgw(sgw_addr, sgw_teid, std::move(packet));
}

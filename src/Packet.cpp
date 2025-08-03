#include "Packet.h"

enum struct DATA_TYPE {
    BYTE = 0,
    SHORT = 1,
    INTEGER = 2,
    FLOAT = 3,
    STRING = 4,
    FILE = 5
};

Packet::Packet() {
    p = enet_packet_create(nullptr, 0, false);
}

Packet::Packet(ENetPacket* pac, int id) {
    p = pac;
    originalID = id;
}

/*
Packet::~Packet() {
    if (p)
        enet_packet_destroy(p);
}*/

Packet::Packet(const void* data, size_t size, int sender) {
	p = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
	originalID = sender;
}

void Packet::append(int type, sol::object data) {
    if (!p) return;
	DATA_TYPE t = (DATA_TYPE)type;

    switch (t) {
        case DATA_TYPE::BYTE: {
            uint8_t value = data.as<uint8_t>();
            enet_packet_resize(p, p->dataLength + sizeof(value));
            memcpy(p->data + p->dataLength - sizeof(value), &value, sizeof(value));
            break;
        }
        case DATA_TYPE::SHORT: {
            uint16_t value = data.as<uint16_t>();
            enet_packet_resize(p, p->dataLength + sizeof(value));
            memcpy(p->data + p->dataLength - sizeof(value), &value, sizeof(value));
            break;
        }
        case DATA_TYPE::INTEGER: {
            int32_t value = data.as<int32_t>();
            enet_packet_resize(p, p->dataLength + sizeof(value));
            memcpy(p->data + p->dataLength - sizeof(value), &value, sizeof(value));
            break;
        }
        case DATA_TYPE::FLOAT: {
            float value = data.as<float>();
            enet_packet_resize(p, p->dataLength + sizeof(value));
            memcpy(p->data + p->dataLength - sizeof(value), &value, sizeof(value));
            break;
        }
        case DATA_TYPE::STRING: {
            std::string value = data.as<std::string>();
            uint16_t length = static_cast<uint16_t>(value.size());
            enet_packet_resize(p, p->dataLength + sizeof(length) + length);
            memcpy(p->data + p->dataLength - (sizeof(length) + length), &length, sizeof(length));
            memcpy(p->data + p->dataLength - length, value.data(), length);
            break;
        }
        case DATA_TYPE::FILE: {
            std::string filePath = data.as<std::string>();

            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) return;

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string fileContent = buffer.str();

            uint32_t fileSize = static_cast<uint32_t>(fileContent.size());
            enet_packet_resize(p, p->dataLength + sizeof(fileSize) + fileSize);
            memcpy(p->data + p->dataLength - (sizeof(fileSize) + fileSize), &fileSize, sizeof(fileSize));
            memcpy(p->data + p->dataLength - fileSize, fileContent.data(), fileSize);

            break;
        }
    }
}

void Packet::destroy() {
    if (p)
        enet_packet_destroy(p);
    p = nullptr;
    pos = 0;
}

int Packet::getSize() {
	return p ? p->dataLength : 0;
}

int Packet::getSenderID() {
	return p ? originalID : -1;
}

sol::object Packet::getNext(int type) {
    if (!p) return 0;

    sol::object o = get(type, pos);

    DATA_TYPE t = (DATA_TYPE)type;
    switch (t) {
    case DATA_TYPE::BYTE:
    {
        uint8_t value;
        pos += sizeof(value);
    }
        break;
    case DATA_TYPE::SHORT:
    {
        uint16_t value;
        pos += sizeof(value);
    }
        break;
    case DATA_TYPE::INTEGER:
    {
        int32_t value;
        pos += sizeof(value);
    }
        break;
    case DATA_TYPE::FLOAT:
    {
        float value;
        pos += sizeof(value);
    }
        break;
    case DATA_TYPE::STRING:
    {
        uint16_t value;
        pos += sizeof(value);
    }
        break;
    }

    return o;
}

void Packet::setPosition(int p) {
    if (p) pos = p;
}

int Packet::getPosition() {
    return p ? pos : 0;
}

sol::object Packet::get(int type, size_t bytePos) {
    if (!p) return 0;
    DATA_TYPE t = (DATA_TYPE)type;
    sol::object result;

    switch (t) {
        case DATA_TYPE::BYTE: {
            uint8_t value;
            memcpy(&value, p->data + bytePos, sizeof(value));
            result = sol::make_object((*lua), value);
            break;
        }
        case DATA_TYPE::SHORT: {
            uint16_t value;
            memcpy(&value, p->data + bytePos, sizeof(value));
            result = sol::make_object((*lua), value);
            break;
        }
        case DATA_TYPE::INTEGER: {
            int32_t value;
            memcpy(&value, p->data + bytePos, sizeof(value));
            result = sol::make_object((*lua), value);
            break;
        }
        case DATA_TYPE::FLOAT: {
            float value;
            memcpy(&value, p->data + bytePos, sizeof(value));
            result = sol::make_object((*lua), value);
            break;
        }
        case DATA_TYPE::STRING: {
            uint16_t length;
            memcpy(&length, p->data + bytePos, sizeof(length));
            bytePos += sizeof(length);
            std::string value(reinterpret_cast<char*>(p->data + bytePos), length);
            result = sol::make_object((*lua), value);
            break;
        }
    }

    return result;
}

bool Packet::writeToFile(int bytePos, std::string path) {
    if (!p) return false;
    if (bytePos < 0 || bytePos >= p->dataLength) return false;

    size_t dataSize = p->dataLength - bytePos - 4;
    std::string dataBuffer(reinterpret_cast<char*>(p->data + bytePos + 4), dataSize);

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile.is_open()) return false;

    outFile.write(dataBuffer.c_str(), dataSize);
    outFile.close();

    return true;
}

void bindPacket() {
    sol::usertype<Packet> bind_type = lua->new_usertype<Packet>("Packet",
        sol::constructors<Packet()>(),
        
        "ID", &Packet::originalID,
        "position", sol::property(&Packet::getPosition, &Packet::setPosition));

    bind_type["append"] = &Packet::append;
    bind_type["get"] = &Packet::get;
    bind_type["getSize"] = &Packet::getSize;
    bind_type["destroy"] = &Packet::destroy;
    bind_type["writeToFile"] = &Packet::writeToFile;
    bind_type["getNext"] = &Packet::getNext;
}

#ifndef IEsk8Device
#include <IEsk8Device.h>
#endif
#include <NRF24L01Library.h>

NRF24L01Lib nrf24;

void initNRF24L01();

class NRF24L01Client : public IEsk8Device
{
public:
  NRF24L01Client()
  {
  }

  void initialise()
  {
  }

  void connect()
  {
  }

  void update() 
  {
    nrf24.update();
  }

  void setOnConnectedEvent(callBack ptr_onConnectedEvent)
  {
    _onConnectedEvent = ptr_onConnectedEvent;
  }
  void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent)
  {
    _onDisconnectedEvent = ptr_onDisconnectedEvent;
  }
  void setOnNotifyEvent(notifyCallBack ptr_onNotifyEvent)
  {
    _onNotifyEvent = ptr_onNotifyEvent;
  }
  void setOnSentEvent(callBack ptr_onSentEvent)
  {
    _onSentEvent = ptr_onSentEvent;
  }

private:
};

#ifndef NRF24L01Client
NRF24L01Client client;
#endif

#define SPI_MOSI  23  // blue
#define SPI_MISO  19  // orange
#define SPI_CLK   18  // yellow
#define SPI_CE 33     // white/purple
#define SPI_CS 26     // green

RF24 radio(SPI_CE, SPI_CS); // ce pin, cs pinRF24Network network();
RF24Network network(radio);
uint16_t peer_addr = 0;

void onDataRecv(uint16_t from)
{
  peer_addr = from;
  client._onNotifyEvent(&nrf24.recv_packet, sizeof(BoardPacket));
}

bool sendPacket() 
{ 
  return nrf24.sendPacket(peer_addr);
}

void initNRF24L01(NRF24L01Lib::Role role)
{
  SPI.begin();
  radio.begin();
  nrf24.begin(&radio, &network, role, onDataRecv);
  radio.setAutoAck(true);
  if (role == NRF24L01Lib::RF24_CLIENT) {
    peer_addr = NRF24L01Lib::RF24_SERVER;
  }
  else {
    peer_addr = NRF24L01Lib::RF24_CLIENT;
  }
}
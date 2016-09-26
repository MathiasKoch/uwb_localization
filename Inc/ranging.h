#include "deca_device_api.h"
#include "deca_regs.h"
#include "decawave.h"
#include "hal.h"
#include "decaTime.h"
#include "device.h" 
#include "mac.h"
#include "deca.h"
#include "rng.h"


// messages used in the ranging protocol
#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
#define BLINK 4
#define RANGING_INIT 5
#define TOKEN_PASS 6
#define TOKEN_RETURN 7

#define MAX_DATA_LEN 90

//Max devices we put in the networkDevices array ! Each device is 74 Bytes in SRAM memory for now.
#define MAX_DEVICES 6


//Default value
//in ms
#define DEFAULT_RESET_PERIOD 600
//in us
#define DEFAULT_REPLY_DELAY_TIME 7000

//sketch type (anchor or tag)
#define TAG 0
#define ANCHOR 1

//default timer delay
#define DEFAULT_TIMER_DELAY 100



class rangingClass {
public:

	rangingClass();
	~rangingClass();

	//variables 
	// data buffer
	uint8_t data[MAX_DATA_LEN];
	
	
	//initialisation
	void    configureNetwork(uint16_t networkId, uint16_t ant_delay);
	void    startAsAnchor(uint8_t address[], uint16_t ant_delay);
	void    startAsTag(uint8_t address[], uint16_t ant_delay);
	bool	addNetworkDevices(device* dev, bool shortAddress);
	bool addNetworkDevices(device* dev);
	void    removeNetworkDevices(short index);
	
	//setters
	void setReplyTime(uint16_t replyDelayTimeUs);
	void setResetPeriod(unsigned long resetPeriod);
	
	//getters 
	uint8_t* getCurrentAddress() { return _currentAddress; };
	
	uint8_t* getCurrentShortAddress() { return _currentShortAddress; };
	
	short getNetworkDevicesNumber() { return _networkDevicesNumber; };
	device* getNetworkDevice(int index);
	
	//ranging functions
	short detectMessageType(uint8_t datas[]);
	void  loop();
	
	
	//Handlers:
	void attachNewDevice(void (* handleNewDevice)(device* distantDevice)) { _handleNewDevice = handleNewDevice; };
	void attachRemovedDevice(void (* handleRemovedDevice)(device* distantDevice)) { _handleRemovedDevice = handleRemovedDevice; };
	void attachNewRange(void (* handleNewRange)(device* distantDevices, short networkDevices)) { _handleNewRange = handleNewRange; };
	void attachNewAnchorRange(void (* handleNewAnchorRange)(float * ranges)) { _handleNewAnchorRange = handleNewAnchorRange; };
	
	
	device* getDistantDevice();
	device* searchDistantDevice(uint8_t shortAddress[]);

	static void handleSent(const dwt_callback_data_t * data);
	static void handleReceived(const dwt_callback_data_t * data);
	
	//FOR DEBUGGING
	//void visualizeDatas(uint8_t datas[]);


private:
	//other devices in the network
	device _networkDevices[MAX_DEVICES];
	short        _networkDevicesNumber;
	short        _lastDistantDevice;
	uint8_t         _currentAddress[8];
	uint8_t         _currentShortAddress[2];
	uint8_t         _lastSentToShortAddress[2];
	mac    _globalMac;
	long         timer;
	short        counterForBlink;
	static const dwt_callback_data_t * _sentCallbackData;
	static const dwt_callback_data_t * _receivedCallbackData;
	decaTime _tx_ant_delay;

	device trueMaster;
	bool tokenHolder = false;
	bool newTokenHolder = false;
	bool trueTag = false;
	short tokenCounter = 0;

	
	//Handlers:
	void (* _handleNewDevice)(device* distantDevice);
	void (* _handleRemovedDevice)(device* distantDevice);
	void (* _handleNewRange)(device* distantDevices, short networkDevices);
	void (* _handleNewAnchorRange)(float * ranges);
	
	//sketch type (tag or anchor)
	short              _type; //0 for tag and 1 for anchor
	// message flow state
	volatile uint8_t    _expectedMsgId;
	// message sent/received state
	static volatile bool _sentAck;
	static volatile bool _receivedAck;
	// protocol error state
	bool          _protocolFailed;
	// watchdog and reset period
	unsigned long    _lastActivity;
	unsigned long    _resetPeriod;
	// reply times (same on both sides for symm. ranging)
	uint16_t     _replyDelayTimeUS;
	//timer Tick delay
	long     _timerDelay;
	// ranging counter (per second)
	//unsigned int     _successRangingCount;
	//unsigned long    _rangingCountPeriod;
		
	
	//methods
	
	void noteActivity();
	void resetInactive();
	
	//global functions:
	void checkForReset();
	void checkForInactiveDevices();
	void copyShortAddress(uint8_t address1[], uint8_t address2[]);
	
	//for ranging protocole (ANCHOR)
	void transmit(uint8_t datas[], uint8_t len);
	void transmitNow(uint8_t datas[], uint8_t len);
	void transmit(uint8_t datas[], decaTime deltatime, uint8_t len);
	void transmitBlink();
	void transmitRangingInit(device* myDistantDevice);
	void transmitPollAck(device* myDistantDevice);
	void transmitRangeReport(device* myDistantDevice);
	void transmitRangeFailed(device* myDistantDevice);
	void transmitToken(device* myDistantDevice);
	void transmitReturnToken(device* myDistantDevice);
	
	//for ranging protocole (TAG)
	void transmitPoll(device* myDistantDevice);
	void transmitRange(device* myDistantDevice);
	
	//methods for range computation
	void computeRangeAsymmetric(device* myDistantDevice, decaTime* myTOF);
	
	void timerTick();
	
};

//extern rangingClass ranging;

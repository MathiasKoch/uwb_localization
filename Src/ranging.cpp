#include "ranging.h"
#include "log.h"
#include "decawave.h"
#include "spi.h"

#ifdef ENABLE_EKF
  	#include "ekf.h"
	extern Ekf EKF;
#endif



volatile bool rangingClass::_sentAck = false;
volatile bool rangingClass::_receivedAck = false;
const dwt_callback_data_t * rangingClass::_sentCallbackData = 0;
const dwt_callback_data_t * rangingClass::_receivedCallbackData = 0;



/* ###########################################################################
 * #### Init and end #######################################################
 * ######################################################################### */

void rangingClass::configureNetwork(uint16_t networkId, uint16_t ant_delay) {
	dwt_config_t config = {
	    .chan = CHANNEL,
	    .prf = PRF,
	    .txPreambLength = DWT_PLEN_128,
	    .rxPAC = DWT_PAC8,
	    .txCode = 9,
	    .rxCode = 9,
	    .nsSFD = 0,
	    .dataRate = DWT_BR_6M8,
	    .phrMode = DWT_PHRMODE_STD,
	    .sfdTO = (129 + 8 - 8),
	};

	dwt_txconfig_t txconfig = {
		.PGdly = 0xC2,
		.power = 0x67676767,
	};

	reset_DW1000();

	dw1000_spi_slow();

	dwt_initialise(DWT_LOADUCODE);

	dw1000_spi_fast();

	dwt_setleds(1);

	dwt_configure(&config);

	dwt_configuretxrf(&txconfig);

	/* Apply default antenna delay value. See NOTE 1 below. */
	dwt_setrxantennadelay((uint16)(ant_delay * 0.56));
	dwt_settxantennadelay((uint16)(ant_delay * 0.44));


	_tx_ant_delay.setTimestamp((uint16)(ant_delay * 0.44));


	//useExtendedFrameLength(false);
	dwt_setsmarttxpower(false);
	//suppressFrameCheck(false);
	//for global frame filtering
	//dwt_enableframefilter(DWT_FF_DATA_EN | DWT_FF_RSVD_EN);
	//for data frame (poll, poll_ack, range, range report, range failed) filtering
	//setFrameFilterAllowData(true);
	//for reserved (blink) frame filtering
	//setFrameFilterAllowReserved(true);
	//setFrameFilterAllowMAC(true);
	//setFrameFilterAllowBeacon(true);
	//setFrameFilterAllowAcknowledgement(true);
	//interruptOnSent(true);
	//interruptOnReceived(true);
	//interruptOnReceiveFailed(true);
	//interruptOnReceiveTimestampAvailable(false);
	//interruptOnAutomaticAcknowledgeTrigger(true);
	dwt_setautorxreenable(false);


	//we need to define a random short address:
	_currentShortAddress[0] = (uint8_t)RNG_Get();
	_currentShortAddress[1] = (uint8_t)RNG_Get();

	uint16_t deviceAddress = _currentShortAddress[0]*256+_currentShortAddress[1];

	MPL_LOGI("My device short address is: 0x%x\n", deviceAddress);
	dwt_setaddress16(deviceAddress);
	dwt_setpanid(networkId);
	
	dwt_setinterrupt(DWT_INT_TFRS | DWT_INT_RFCG | DWT_INT_RPHE | DWT_INT_RFCE | DWT_INT_RFSL, true);


	_resetPeriod      = DEFAULT_RESET_PERIOD;
	// reply times (same on both sides for symm. ranging)
	_replyDelayTimeUS = DEFAULT_REPLY_DELAY_TIME;
	//we set our timer delay
	_timerDelay       = DEFAULT_TIMER_DELAY;


	// attach callback for (successfully) sent and received messages
	decamutexon();
  	dwt_setcallbacks(handleSent, handleReceived);
  	decamutexoff(true);	

	// for first time ranging frequency computation
	//_rangingCountPeriod = HAL_GetTick();

}


void rangingClass::startAsAnchor(uint8_t address[], uint16_t ant_delay) {
	//write the address on the DW1000 chip
	dwt_seteui(address);

	
	//we configure the network for mac filtering
	//(network ID, Antenna delay)
	configureNetwork(0xDECA, ant_delay);
	
	//defined type as anchor
	_type = ANCHOR;
	tokenHolder = false;
	trueTag = false;
	dwt_rxenable(0);

	
	MPL_LOGI("### ANCHOR ###\n");
	
}

void rangingClass::startAsTag(uint8_t address[], uint16_t ant_delay) {
	//write the address on the DW1000 chip
	dwt_seteui(address);
	
	//we configure the network for mac filtering
	//(network ID, Antenna delay)
	configureNetwork(0xDECA, ant_delay);
	
	//defined type as anchor
	_type = TAG;
	tokenHolder = true;
	trueTag = true;
	
	MPL_LOGI("### TAG ### \n");
}

bool rangingClass::addNetworkDevices(device* dev, bool shortAddress) {
	//we test our network devices array to check
	//we don't already have it
	for(short i= 0; i < _networkDevicesNumber; i++) {
		if(_networkDevices[i].isAddressEqual(dev) && !shortAddress) {
			//the device already exists
			return false;
		}else if(_networkDevices[i].isShortAddressEqual(dev) && shortAddress) {
			//the device already exists
			return false;
		}
		
	}
	
	memcpy(&_networkDevices[_networkDevicesNumber], dev, sizeof(device));
	_networkDevices[_networkDevicesNumber].setIndex(_networkDevicesNumber);
	if(_handleNewDevice != 0) {
		(*_handleNewDevice)(&_networkDevices[_networkDevicesNumber]);
	}
	_networkDevicesNumber++;
	return true;
}

bool rangingClass::addNetworkDevices(device* dev) {
	//we test our network devices array to check
	//we don't already have it
	for(short i = 0; i < _networkDevicesNumber; i++) {
		if(_networkDevices[i].isAddressEqual(dev) && _networkDevices[i].isShortAddressEqual(dev)) {
			//the device already exists
			return false;
		}
		
	}
	
	if(_type == ANCHOR){ //for now let's start with 1 TAG
		_networkDevicesNumber = 0;
	}
	memcpy(&_networkDevices[_networkDevicesNumber], dev, sizeof(device));
	_networkDevices[_networkDevicesNumber].setIndex(_networkDevicesNumber);
	if(_handleNewDevice != 0) {
		(*_handleNewDevice)(&_networkDevices[_networkDevicesNumber]);
	}
	_networkDevicesNumber++;
	return true;
}

void rangingClass::removeNetworkDevices(short index) {
	//if we have just 1 element
	if(_networkDevicesNumber == 1) {
		_networkDevicesNumber = 0;
	}else if(index == _networkDevicesNumber-1){ //if we delete the last element
		_networkDevicesNumber--;
	}else {
		//we translate all the element wich are after the one we want to delete.
		for(int i = index; i < _networkDevicesNumber-1; i++) {
			memcpy(&_networkDevices[i], &_networkDevices[i+1], sizeof(device));
			_networkDevices[i].setIndex(i);
		}
		_networkDevicesNumber--;
	}
}

/* ###########################################################################
 * #### Setters and Getters ##################################################
 * ######################################################################### */

//setters
void rangingClass::setReplyTime(uint16_t replyDelayTimeUs) { 
	_replyDelayTimeUS = replyDelayTimeUs; 
}

void rangingClass::setResetPeriod(unsigned long resetPeriod) {
	_resetPeriod = resetPeriod; 
}


device* rangingClass::searchDistantDevice(uint8_t shortAddress[]) {
	//we compare the 2 bytes address with the others
	for(int i = 0; i < _networkDevicesNumber; i++) {
		if(memcmp(shortAddress, _networkDevices[i].getByteShortAddress(), 2) == 0) {
			//we have found our device !
			return &_networkDevices[i];
		}
	}
	return NULL;
}

device* rangingClass::getNetworkDevice(int kalmanID) {
	//we get the device which correspond to the message which was sent (need to be filtered by MAC address)
	for(int i = 0; i < getNetworkDevicesNumber(); i++){
		if(_networkDevices[i].getKalmanID() == kalmanID)
			return &_networkDevices[i];
	}
	return NULL;
}

device* rangingClass::getDistantDevice() {
	//we get the device which correspond to the message which was sent (need to be filtered by MAC address)
	return &_networkDevices[_lastDistantDevice];
}


/* ###########################################################################
 * #### Public methods #######################################################
 * ######################################################################### */

void rangingClass::checkForReset() {
	if(!_sentAck && !_receivedAck) {
		// check if inactive
		if(HAL_GetTick()-_lastActivity > _resetPeriod) {
			resetInactive();
		}
	}
}

void rangingClass::checkForInactiveDevices() {
	int cnt = _networkDevicesNumber;
	for(int i = cnt-1; i >= 0; i--) {
		//MPL_LOGI("Activity 0x%x - {%d}\n", _networkDevices[i].getShortAddress(), HAL_GetTick()-_networkDevices[i].getActivity());
		if(_networkDevices[i].isInactive()) {
			//we need to delete the device from the array:
			if(_handleRemovedDevice != 0){
				(*_handleRemovedDevice)(&(_networkDevices[i]));
			}
			removeNetworkDevices(i);
			if(_type == ANCHOR){
				dwt_rxenable(0);
			}
		}
	}
}

short rangingClass::detectMessageType(uint8_t datas[]) {
	if(datas[0] == FC_1_BLINK) {
		return BLINK;
	}else if(datas[0] == FC_1 && datas[1] == FC_2) {
		//we have a long MAC frame message (ranging init)
		return datas[LONG_MAC_LEN];
	}else if(datas[0] == FC_1 && datas[1] == FC_2_SHORT) {
		//we have a short mac frame message (poll, range, range report, etc..)
		return datas[SHORT_MAC_LEN];
	}
	return 0;
}


void rangingClass::loop() {
	// TODO: Handle errors!
	//we check if needed to reset !

	checkForReset();
	long time_ = HAL_GetTick();
	if(time_ - timer > _timerDelay) {
		timer = time_;
		timerTick();
	}
	

	if(_sentAck) {
		_sentAck = false;
		
		
		int messageType = detectMessageType(data);
		
		if(messageType != POLL_ACK && messageType != POLL && messageType != RANGE)
			return;
		
		
		//A msg was sent. We launch the ranging protocol when a message was sent
		if(_type == ANCHOR) {
			if(messageType == POLL_ACK) {
				device* myDistantDevice = searchDistantDevice(_lastSentToShortAddress);
				readtxtimestamp(myDistantDevice->timePollAckSent);
			}
		}else if(_type == TAG) {
			if(messageType == POLL) {
				decaTime timePollSent;
				readtxtimestamp(timePollSent);
				//if the last device we send the POLL is broadcast:
				if(_lastSentToShortAddress[0] == 0xFF && _lastSentToShortAddress[1] == 0xFF) {
					//we save the value for all the devices !
					for(short i = 0; i < _networkDevicesNumber; i++) {
						_networkDevices[i].timePollSent = timePollSent;
					}
				}else{
					//we search the device associated with the last send address
					device* myDistantDevice = searchDistantDevice(_lastSentToShortAddress);
					//we save the value just for one device
					myDistantDevice->timePollSent = timePollSent;
				}
			}else if(messageType == RANGE) {
				decaTime timeRangeSent;
				readtxtimestamp(timeRangeSent);
				//if the last device we send the POLL is broadcast:
				if(_lastSentToShortAddress[0] == 0xFF && _lastSentToShortAddress[1] == 0xFF) {
					//we save the value for all the devices !
					for(short i = 0; i < _networkDevicesNumber; i++) {
						_networkDevices[i].timeRangeSent = timeRangeSent;
					}
				}else{
					//we search the device associated with the last send address
					device* myDistantDevice = searchDistantDevice(_lastSentToShortAddress);
					//we save the value just for one device
					myDistantDevice->timeRangeSent = timeRangeSent;
				}
			}
		}
	}
	
	//check for new received message
	if(_receivedAck) {
		_receivedAck = false;
		
		//we read the datas from the modules:
		// get message and parse
		dwt_readrxdata(data, _receivedCallbackData->datalength, 0);		
		
		int messageType = detectMessageType(data);
		
		//we have just received a BLINK message from tag
		if(messageType == BLINK && _type == ANCHOR) {
			MPL_LOGE("Receied BLINK\n");

			uint8_t address[8];
			uint8_t shortAddress[2];

			_globalMac.decodeBlinkFrame(data, address, shortAddress);
			//we crate a new device with th tag
			device myTag(address, shortAddress);
			
			if(addNetworkDevices(&myTag)) {
				//we reply by the transmit ranging init message

				_expectedMsgId = TOKEN_PASS;
				transmitRangingInit(&myTag);
				myTag.noteActivity();
				noteActivity();
			}else{
				_expectedMsgId = POLL;
				dwt_rxenable(0);
			}
		}else if(messageType == RANGING_INIT && _type == TAG) {
			uint8_t address[2];
			//MPL_LOGE("Receied RANGING_INIT\n");


			_globalMac.decodeLongMACFrame(data, address);
			//we crate a new device with the anchor
			device myAnchor(address, true);
			
			if(addNetworkDevices(&myAnchor, true)){
				myAnchor.noteActivity();
				
				transmitToken(&myAnchor);

				// TODO: Add timeout here in case the new tokenHolder crashes/goes offline while holding the token!
			}
			

		}else {
			//we have a short mac layer frame !
			uint8_t address[2];
			_globalMac.decodeShortMACFrame(data, address);
			
			
			
			//we get the device which correspond to the message which was sent (need to be filtered by MAC address)
			device* myDistantDevice = searchDistantDevice(address);
			
			
			if(myDistantDevice == NULL) {
				//we don't have the short address of the device in memory
				//MPL_LOGI("Not found \t unknown address: 0x%x\n", address[1]<<8 | address[0]);
				dwt_rxenable(0);
				return;
			}
			
			
			//then we proceed to range protocol
			if(_type == ANCHOR) {


				// Always allow reception of a TOKEN_PASS
				if(messageType == TOKEN_PASS){

					uint8_t shortAddress[2];
					memcpy(shortAddress, data+SHORT_MAC_LEN+2, 2);

					if(!trueTag){
						trueMaster = *myDistantDevice;
						trueMaster.noteActivity();
					}

					MPL_LOGE("Received TOKEN_PASS! [0x%x = 0x%x]\n",shortAddress[1]*256+shortAddress[0], _currentShortAddress[1]*256+_currentShortAddress[0]);

					if(shortAddress[0] == _currentShortAddress[0] && shortAddress[1] == _currentShortAddress[1]){
  						//led_on(LED_1);


						int numberDevices = 0;
						memcpy(&numberDevices, data+SHORT_MAC_LEN+1, 1);

						// remove all known devices
						removeNetworkDevices(myDistantDevice->getIndex());


						for(int i = 0; i < numberDevices; i++) {
							//we grab the mac address of each devices:
							memcpy(shortAddress, data+SHORT_MAC_LEN+4+i*2, 2);


							device myAnchor(shortAddress, true);
							if(addNetworkDevices(&myAnchor, true)){
								myAnchor.noteActivity();
								MPL_LOGI("Added [0x%x] in TOKEN_PASS (ME token holder!) - {%d}\n", myAnchor.getShortAddress(), HAL_GetTick()-trueMaster.getActivity());

								//MPL_LOGI("Added device! 0x%x\n", myAnchor.getShortAddress());
							}
						}


						_type = TAG;
						tokenHolder = true;

						// Number of tries to get hold of other anchors will be (21-counterForBlink) = 2
						counterForBlink = 19;
						return;
					}else{

						device myAnchor(shortAddress, true);
						if(addNetworkDevices(&myAnchor, true)){
							myAnchor.noteActivity();
							MPL_LOGI("Added [0x%x] in TOKEN_PASS (NewTokenHolder) - {%d}\n", myAnchor.getShortAddress(), HAL_GetTick()-trueMaster.getActivity());
							//MPL_LOGI("Added device! 0x%x\n", myAnchor.getShortAddress());
							newTokenHolder = true;
						}


						_expectedMsgId = POLL;
						dwt_rxenable(0);
						return;
					}
				}

				if(messageType != _expectedMsgId) {
					// unexpected message, start over again (except if already POLL)
					//MPL_LOGI("Unexpected msg: %d\t%d\n",messageType,_expectedMsgId);
					_protocolFailed = true;
					dwt_rxenable(0);
					return;
				}

				if(messageType == POLL) {


					//we receive a POLL which is a broacast message
					//we need to grab info about it
					int numberDevices = 0;
					memcpy(&numberDevices, data+SHORT_MAC_LEN+1, 1);
					
					for(int i = 0; i < numberDevices; i++) {
						//we need to test if this value is for us:
						//we grab the mac address of each devices:
						uint8_t shortAddress[2];
						memcpy(shortAddress, data+SHORT_MAC_LEN+2+i*4, 2);
						
						//we test if the short address is our address
						if(shortAddress[0] == _currentShortAddress[0] && shortAddress[1] == _currentShortAddress[1]){


							//we grab the replytime which is for us
							uint16_t replyTime;
							memcpy(&replyTime, data+SHORT_MAC_LEN+2+2+4*i, 2);
							//we configure our replyTime;
							_replyDelayTimeUS = replyTime;


							//MPL_LOGE("\n\nReceived POLL from [0x%x] - Reply in %d\n", myDistantDevice->getShortAddress(), _replyDelayTimeUS);
							
							// on POLL we (re-)start, so no protocol failure
							_protocolFailed = false;
							
							readrxtimestamp(myDistantDevice->timePollReceived);
							//we note activity for our device:
							myDistantDevice->noteActivity();
							if(!trueTag){
								trueMaster.noteActivity();
							}
							//we indicate our next receive message for our ranging protocole
							_expectedMsgId = RANGE;
							transmitPollAck(myDistantDevice);
							noteActivity();
							
							return;
						}
					}
				}else if(messageType == RANGE) {

					//we receive a RANGE which is a broacast message
					//we need to grab info about it

					//MPL_LOGE("Received RANGE from [0x%x] - Reply in %d\n", myDistantDevice->getShortAddress(), _replyDelayTimeUS);

					int numberDevices = 0;
					memcpy(&numberDevices, data+SHORT_MAC_LEN+1, 1);
					
					
					for(int i = 0; i < numberDevices; i++) {
						//we need to test if this value is for us:
						//we grab the mac address of each devices:
						uint8_t shortAddress[2];
						memcpy(shortAddress, data+SHORT_MAC_LEN+2+i*17, 2);
						
						//we test if the short address is our address
						if(shortAddress[0] == _currentShortAddress[0] && shortAddress[1] == _currentShortAddress[1]) {

							//we grab the replytime wich is for us
							readrxtimestamp(myDistantDevice->timeRangeReceived);

							//we note activity for our device:
							myDistantDevice->noteActivity();
							if(!trueTag){
								trueMaster.noteActivity();
							}
							noteActivity();
							_expectedMsgId = POLL;
							
							if(!_protocolFailed) {
								//MPL_LOGI("Received RANGE from 0x%x\n", myDistantDevice->getShortAddress());
								
								myDistantDevice->timePollSent.setTimestamp(data+SHORT_MAC_LEN+4+17*i);
								myDistantDevice->timePollAckReceived.setTimestamp(data+SHORT_MAC_LEN+9+17*i);
								myDistantDevice->timeRangeSent.setTimestamp(data+SHORT_MAC_LEN+14+17*i);

								// (re-)compute range as two-way ranging is done
								decaTime myTOF;
								computeRangeAsymmetric(myDistantDevice, &myTOF); // CHOSEN RANGING ALGORITHM
								
								float distance = myTOF.getAsMeters();
								
								myDistantDevice->setRXPower(getReceivePower());
								myDistantDevice->setRange(distance);
								
								myDistantDevice->setFPPower(getFirstPathPower());
								myDistantDevice->setQuality(getReceiveQuality());


								
								//we send the range to TAG
								transmitRangeReport(myDistantDevice);
								
								//we have finished our range computation. We send the corresponding handler
								_lastDistantDevice = myDistantDevice->getIndex();

								if(newTokenHolder){
									newTokenHolder = false;
									MPL_LOGI("Removed [0x%x] in RANGE - {%d}\n", myDistantDevice->getShortAddress(), HAL_GetTick()-trueMaster.getActivity());
									removeNetworkDevices(myDistantDevice->getIndex());
								}


								/*if(_handleNewRange != 0) {
									(*_handleNewRange)(myDistantDevice);
								}*/
							}else {
								transmitRangeFailed(myDistantDevice);
							}	
							return;
						}
					}
				}
			}else if(_type == TAG) {
				// get message and parse
				if(messageType != _expectedMsgId) {
					// unexpected message, start over again
					dwt_rxenable(0);
					return;
				}



				if(messageType == POLL || messageType == RANGE) {
					myDistantDevice->noteActivity();

				}else if(messageType == POLL_ACK) {

					readrxtimestamp(myDistantDevice->timePollAckReceived);
					//we note activity for our device:
					myDistantDevice->noteActivity();
					if(!trueTag){
						trueMaster.noteActivity();
					}
					//MPL_LOGE("Received POLL_ACK from 0x%x (%d)\n", myDistantDevice->getShortAddress(), myDistantDevice->getIndex());
					//in the case the message come from our last device:
					if(myDistantDevice->getIndex() == _networkDevicesNumber-1) {
						_expectedMsgId = RANGE_REPORT;
						//and transmit the next message (range) of the ranging protocole (in broadcast)
						transmitRange(NULL);
					}else{
						dwt_rxenable(0);
					}
				}else if(messageType == RANGE_REPORT) {
					//MPL_LOGE("Received RANGE_REPORT from 0x%x (%d)\n", myDistantDevice->getShortAddress(), myDistantDevice->getIndex());

					
					float curRange;
					memcpy(&curRange, data+1+SHORT_MAC_LEN, 4);
					float curRXPower;
					memcpy(&curRXPower, data+5+SHORT_MAC_LEN, 4);

					//we note activity for our device:
					myDistantDevice->noteActivity();
					if(!trueTag){
						trueMaster.noteActivity();
					}

					//we have a new range to save !
					myDistantDevice->setRange(curRange);
					myDistantDevice->setRXPower(curRXPower);
					
					//MPL_LOGI("0x%x - %f [m]\n", myDistantDevice->getShortAddress(), curRange);

					
					//We can call our handler !
					//we have finished our range computation. We send the corresponding handler
					_lastDistantDevice = myDistantDevice->getIndex();

					

					if(_lastDistantDevice == _networkDevicesNumber-1) {
						if(!trueTag){
							// I used to be an anchor! return to my spot!
							//MPL_LOGI("Transmitting TOKEN_RETURN to 0x%x\n", trueMaster.getShortAddress());

							transmitReturnToken(&trueMaster);

							// Clear all known network devices!
							int cnt = _networkDevicesNumber;
							for(int i = cnt-1; i >= 0; i--) {
								MPL_LOGI("Removed [0x%x] in TOKEN_RETURN (devices: %d) - {%d}\n", _networkDevices[i].getShortAddress(), _networkDevicesNumber, HAL_GetTick()-trueMaster.getActivity());

								removeNetworkDevices(i);
							}

							if(addNetworkDevices(&trueMaster)){
								trueMaster.noteActivity();
							}
							
							tokenHolder = false;
							_type = ANCHOR;
							_expectedMsgId = POLL;
							dwt_rxenable(0);
						}else if(trueTag){
							// I am the rightfull tag, update the EKF
							if(_handleNewRange != 0) {
								(*_handleNewRange)(_networkDevices, _networkDevicesNumber);
							}
						}					
					}else{
						dwt_rxenable(0);
					}	

				}else if(messageType == TOKEN_RETURN) {

					// I am now the tokenHolder again!
					tokenHolder = true;

					//MPL_LOGE("Received TOKEN_RETURN! {%d}\n", HAL_GetTick()-myDistantDevice->getActivity());
					//we note activity for our device:
					myDistantDevice->noteActivity();

					// Poll right away
					//counterForBlink = 1;

					// Unpack range information between anchors
					int numberDevices = 0;
					memcpy(&numberDevices, data+SHORT_MAC_LEN+1, 1);

					


					float ranges[((MAX_DEVICES-1)*MAX_DEVICES)/2];
					int Rcnt = 0;

					for(int i = 0; i < ((MAX_DEVICES-1)*MAX_DEVICES)/2; i++)
						ranges[i] = -1;
					
					for(int i = 0; i < numberDevices; i++) {

						// This range is between 'distantDevice' & 'anchor', where 'anchor' is found here
						uint8_t shortAddress[2];
						memcpy(&shortAddress, data+SHORT_MAC_LEN+2+6*i, 2);

						device* anchor = searchDistantDevice(shortAddress);

						/* We could note activity on this anchor device, as it was active at some point? 
						 * But I don't think it is necessary
						 */
						anchor->noteActivity();
						

						float curRange;
						memcpy(&curRange, data+SHORT_MAC_LEN+2+2+6*i, 4);

						//MPL_LOGI("Range from [0x%x] (%d) to [0x%x] (%d): %f\n", myDistantDevice->getShortAddress(), myDistantDevice->getKalmanID(), anchor->getShortAddress(), anchor->getKalmanID(), curRange);


						// TODO: Kalman IDs are no good for this!, should be EKF.getStateQueue(kalmanID) [NOT available from here! (Static version?)]
						int cnt = _networkDevicesNumber;
						Rcnt = 0;
						for(int i = 0; i < _networkDevicesNumber-1; i++){
							for(int j = 1; j < cnt; j++){
#ifdef STM32F10X_MD
								//if(i == EKF.getStateQueue(anchor->getKalmanID()) && (i+j) == EKF.getStateQueue(myDistantDevice->getKalmanID())){
								if(i == anchor->getKalmanID() && (i+j) == myDistantDevice->getKalmanID()){
									ranges[Rcnt] = curRange;
								}
#endif
								Rcnt++;
							}
							cnt--;
						}
					}

					if(_networkDevicesNumber > 3){
						if(_handleNewAnchorRange != 0) {
							(*_handleNewAnchorRange)(ranges);
						}
					}

				}else if(messageType == RANGE_FAILED) {
					//not needed as we have a timer;
					return;
				}
			}
		}
		
	}
}


/* ###########################################################################
 * #### Private methods and Handlers for transmit & Receive reply ############
 * ######################################################################### */


void rangingClass::handleSent(const dwt_callback_data_t * data) {
	// status change on sent success
	_sentAck = true;
	_sentCallbackData = data;
}

void rangingClass::handleReceived(const dwt_callback_data_t * data) {
	// status change on received success
	_receivedAck = true;
	_receivedCallbackData = data;
}


void rangingClass::noteActivity() {
	// update activity timestamp, so that we do not reach "resetPeriod"
	_lastActivity = HAL_GetTick();
}

void rangingClass::resetInactive() {
	//if inactive
	if(_type == ANCHOR) {
		_expectedMsgId = POLL;
		//dwt_rxenable(0);
		MPL_LOGI("resetting on the count of {%d}\n", HAL_GetTick()-_lastActivity);
	}
	noteActivity();
}

void rangingClass::timerTick() {
	if(_networkDevicesNumber > 0 && counterForBlink != 0 && counterForBlink != 10) {
		if(_type == TAG && tokenHolder) {
			_expectedMsgId = POLL_ACK;
			//send a broadcast poll

			transmitPoll(NULL);
			if(!trueTag)
				trueMaster.noteActivity();
		}
	}else if(counterForBlink == 0) {
		if(tokenHolder && _type == TAG){
			if(trueTag) {
				transmitBlink();
			}else if(!trueTag){
				// Timeout on the TOKEN! Time to return!
				//MPL_LOGI("Transmitting TOKEN_RETURN to 0x%x \t TIMEOUT\n", trueMaster.getShortAddress());

				for(int i = 0; i < _networkDevicesNumber; i++)
					_networkDevices[i].setRange(-1);

				transmitReturnToken(&trueMaster);

				// Clear all known network devices!
				int cnt = _networkDevicesNumber;
				for(int i = cnt-1; i >= 0; i--) {
					MPL_LOGI("Removed [0x%x] in TOKEN_RETURN (TIMEOUT) (devices: %d) - {%d}\n", _networkDevices[i].getShortAddress(), _networkDevicesNumber, HAL_GetTick()-trueMaster.getActivity());

					removeNetworkDevices(i);
				}

				trueMaster.noteActivity();
				addNetworkDevices(&trueMaster);
				
				_type = ANCHOR;
				tokenHolder = false;
				_expectedMsgId = POLL;
				dwt_rxenable(0);
			}
		}
		//check for inactive devices if we are a TAG or ANCHOR (Exclude checking while passed the token)
		if(!(trueTag && !tokenHolder) && !newTokenHolder){
			checkForInactiveDevices();
		}
	}else if(_networkDevicesNumber > 0 && counterForBlink == 10 ){
		if(trueTag && tokenHolder) {
			/*device anchor = _networkDevices[tokenCounter];
			transmitToken(&anchor);
			tokenCounter--;
			if(tokenCounter < 0)
				tokenCounter = _networkDevicesNumber-1;*/
		}else if(trueTag){
			// Timeout occured on token!
			tokenHolder = true;
		}
	}
	if(counterForBlink++ > 20) {
		counterForBlink = 0;		
	}
}


void rangingClass::copyShortAddress(uint8_t address1[], uint8_t address2[]) {
	*address1     = *address2;
	*(address1+1) = *(address2+1);
}

/* ###########################################################################
 * #### Methods for ranging protocole   ######################################
 * ######################################################################### */


void rangingClass::transmit(uint8_t datas[], uint8_t len) {
	dwt_writetxdata(len+CRC_LEN, datas, 0);

    dwt_writetxfctrl(len+CRC_LEN, 0);
    dwt_starttx(DWT_RESPONSE_EXPECTED | DWT_START_TX_DELAYED);
}

void rangingClass::transmitNow(uint8_t datas[], uint8_t len) {
	dwt_writetxdata(len+CRC_LEN, datas, 0);

    dwt_writetxfctrl(len+CRC_LEN, 0);
    dwt_starttx(DWT_RESPONSE_EXPECTED | DWT_START_TX_IMMEDIATE);
}


void rangingClass::transmit(uint8_t datas[], decaTime deltatime, uint8_t len) {
	uint8_t ts[LEN_STAMP];
	dwt_readsystime(ts);
	decaTime systime = decaTime(ts);
	systime += deltatime;
    dwt_setdelayedtrxtime((systime.getTimestamp()>>8) & 0xFFFFFFFE);

	dwt_writetxdata(len+CRC_LEN, datas, 0);

    dwt_writetxfctrl(len+CRC_LEN, 0);
    dwt_starttx(DWT_RESPONSE_EXPECTED | DWT_START_TX_DELAYED);
}

void rangingClass::transmitBlink() {
	dwt_forcetrxoff();
	MPL_LOGE("Sending BLINK\n");
	_globalMac.generateBlinkFrame(data, _currentAddress, _currentShortAddress);
	transmitNow(data, BLINK_LEN+1);
}

void rangingClass::transmitRangingInit(device* myDistantDevice) {
	//we generate the mac frame for a ranging init message
	_globalMac.generateLongMACFrame(data, _currentShortAddress, myDistantDevice->getByteAddress());
	//we define the function code
	data[LONG_MAC_LEN] = RANGING_INIT;
	
	copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());
	
	transmitNow(data, LONG_MAC_LEN+1);
}

void rangingClass::transmitToken(device* myDistantDevice) {
	_expectedMsgId = TOKEN_RETURN;
	tokenHolder = false;


	// Not 100% sure??
	dwt_forcetrxoff();

	uint8_t shortBroadcast[2] = {0xFF, 0xFF};
	_globalMac.generateShortMACFrame(data, _currentShortAddress, shortBroadcast);
	data[SHORT_MAC_LEN]   = TOKEN_PASS;
	data[SHORT_MAC_LEN+1]   = _networkDevicesNumber-1;


	//we write the short address of our device:
	memcpy(data+SHORT_MAC_LEN+2, myDistantDevice->getByteShortAddress(), 2);

	int counter = 0;

	for(int i = 0; i < _networkDevicesNumber; i++){
		if(_networkDevices[i].getShortAddress() != myDistantDevice->getShortAddress())
			memcpy(data+SHORT_MAC_LEN+2+2+(2*counter++), _networkDevices[i].getByteShortAddress(), 2);
	}

	copyShortAddress(_lastSentToShortAddress, shortBroadcast);
		
	//MPL_LOGW("Transmitting TOKEN_PASS to 0x%x - {%d}\n", myDistantDevice->getShortAddress(), HAL_GetTick()-myDistantDevice->getActivity());

	transmitNow(data, SHORT_MAC_LEN+2+2+(2*_networkDevicesNumber-1));
}

void rangingClass::transmitReturnToken(device* myDistantDevice) {
	//dwt_forcetrxoff();
	_globalMac.generateShortMACFrame(data, _currentShortAddress, myDistantDevice->getByteShortAddress());
	data[SHORT_MAC_LEN] = TOKEN_RETURN;
	//led_off(LED_1);

	data[SHORT_MAC_LEN+1] = _networkDevicesNumber;
	MPL_LOGW("Transmit TOKEN_RETURN to 0x%x - {%d}\n", myDistantDevice->getShortAddress(), HAL_GetTick()-trueMaster.getActivity());


	for(short i = 0; i < _networkDevicesNumber; i++) {

		memcpy(data+SHORT_MAC_LEN+2+6*i, _networkDevices[i].getByteShortAddress(), 2);

		float currRange = _networkDevices[i].getRange();

		memcpy(data+SHORT_MAC_LEN+2+2+6*i, &currRange, 4);
	}

	copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());

	transmitNow(data, SHORT_MAC_LEN+2+6*_networkDevicesNumber);
}

void rangingClass::transmitPoll(device* myDistantDevice) {
	dwt_forcetrxoff();
	//MPL_LOGW("\nTransmit POLL\n");

	//if(myDistantDevice == NULL) {
		//we need to set our timerDelay:
		_timerDelay = DEFAULT_TIMER_DELAY+(int)(_networkDevicesNumber*3*DEFAULT_REPLY_DELAY_TIME/1000);
		
		uint8_t shortBroadcast[2] = {0xFF, 0xFF};
		_globalMac.generateShortMACFrame(data, _currentShortAddress, shortBroadcast);
		data[SHORT_MAC_LEN]   = POLL;
		//we enter the number of devices
		data[SHORT_MAC_LEN+1] = _networkDevicesNumber;
		
		for(short i = 0; i < _networkDevicesNumber; i++) {
			//each devices have a different reply delay time.
			_networkDevices[i].setReplyTime((2*i+1)*DEFAULT_REPLY_DELAY_TIME);

			//we write the short address of our device:
			memcpy(data+SHORT_MAC_LEN+2+4*i, _networkDevices[i].getByteShortAddress(), 2);
			
			//we add the replyTime
			uint16_t replyTime = _networkDevices[i].getReplyTime();
			memcpy(data+SHORT_MAC_LEN+2+2+4*i, &replyTime, 2);
			
		}
		
		copyShortAddress(_lastSentToShortAddress, shortBroadcast);
		
	/*}else{
		//we redefine our default_timer_delay for just 1 device;
		_timerDelay = DEFAULT_TIMER_DELAY;
		
		_globalMac.generateShortMACFrame(data, _currentShortAddress, myDistantDevice->getByteShortAddress());
		
		data[SHORT_MAC_LEN]   = POLL;
		data[SHORT_MAC_LEN+1] = 1;
		uint16_t replyTime = test++;//myDistantDevice->getReplyTime();
		memcpy(data+SHORT_MAC_LEN+2, &replyTime, 2);
		
		copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());
	}*/

	transmitNow(data, SHORT_MAC_LEN+2+4*_networkDevicesNumber);
}


void rangingClass::transmitPollAck(device* myDistantDevice) {
	//MPL_LOGW("Transmit POLL_ACK\n");

	_globalMac.generateShortMACFrame(data, _currentShortAddress, myDistantDevice->getByteShortAddress());
	data[SHORT_MAC_LEN] = POLL_ACK;
	// delay the same amount as ranging tag
	decaTime deltaTime = decaTime(_replyDelayTimeUS, DW_MICROSECONDS);

	copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());


	transmit(data, deltaTime, SHORT_MAC_LEN+1);
}

void rangingClass::transmitRange(device* myDistantDevice) {
	//transmit range need to accept broadcast for multiple anchor
	//MPL_LOGW("Transmit RANGE to [0x%x]\n", myDistantDevice->getShortAddress());

	uint8_t len = 0;
	if(myDistantDevice == NULL) {
		//we need to set our timerDelay:
		_timerDelay = DEFAULT_TIMER_DELAY+(int)(_networkDevicesNumber*3*DEFAULT_REPLY_DELAY_TIME/1000);
		
		uint8_t shortBroadcast[2] = {0xFF, 0xFF};
		_globalMac.generateShortMACFrame(data, _currentShortAddress, shortBroadcast);
		data[SHORT_MAC_LEN]   = RANGE;
		//we enter the number of devices
		data[SHORT_MAC_LEN+1] = _networkDevicesNumber;
		
		// delay sending the message and remember expected future sent timestamp
		decaTime deltaTime     = decaTime(DEFAULT_REPLY_DELAY_TIME, DW_MICROSECONDS);

		uint8_t ts[LEN_STAMP];
		dwt_readsystime(ts);
		decaTime systime = decaTime(ts);
		systime += deltaTime;
		dwt_setdelayedtrxtime((systime.getTimestamp()>>8) & 0xFFFFFFFE);

		decaTime timeRangeSent = systime + _tx_ant_delay;
		
		for(uint8_t i = 0; i < _networkDevicesNumber; i++) {
			//we write the short address of our device:
			memcpy(data+SHORT_MAC_LEN+2+17*i, _networkDevices[i].getByteShortAddress(), 2);
			
			
			//we get the device which correspond to the message which was sent (need to be filtered by MAC address)
			_networkDevices[i].timeRangeSent = timeRangeSent;
			_networkDevices[i].timePollSent.getTimestamp(data+SHORT_MAC_LEN+4+17*i);
			_networkDevices[i].timePollAckReceived.getTimestamp(data+SHORT_MAC_LEN+9+17*i);
			_networkDevices[i].timeRangeSent.getTimestamp(data+SHORT_MAC_LEN+14+17*i);
			
		}
		copyShortAddress(_lastSentToShortAddress, shortBroadcast);

		len = SHORT_MAC_LEN+2+(2+LEN_STAMP*3)*_networkDevicesNumber;
		
	}else{
		_globalMac.generateShortMACFrame(data, _currentShortAddress, myDistantDevice->getByteShortAddress());
		data[SHORT_MAC_LEN] = RANGE;
		// delay sending the message and remember expected future sent timestamp
		decaTime deltaTime = decaTime(_replyDelayTimeUS, DW_MICROSECONDS);
		//we get the device which correspond to the message which was sent (need to be filtered by MAC address)
		uint8_t ts[LEN_STAMP];
		dwt_readsystime(ts);
		decaTime systime(ts);
		systime += deltaTime;
		dwt_setdelayedtrxtime((systime.getTimestamp()>>8) & 0xFFFFFFFE);

		myDistantDevice->timeRangeSent = systime + _tx_ant_delay;
		myDistantDevice->timePollSent.getTimestamp(data+1+SHORT_MAC_LEN);
		myDistantDevice->timePollAckReceived.getTimestamp(data+6+SHORT_MAC_LEN);
		myDistantDevice->timeRangeSent.getTimestamp(data+11+SHORT_MAC_LEN);
		copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());

		len = SHORT_MAC_LEN+1+LEN_STAMP*3;
	}

	transmit(data, len);
}


void rangingClass::transmitRangeReport(device* myDistantDevice) {
	//MPL_LOGW("Transmit RANGE_REPORT\n");

	_globalMac.generateShortMACFrame(data, _currentShortAddress, myDistantDevice->getByteShortAddress());
	data[SHORT_MAC_LEN] = RANGE_REPORT;
	// write final ranging result
	float curRange   = myDistantDevice->getRange();
	float curRXPower = myDistantDevice->getRXPower();
	//We add the Range and then the RXPower
	memcpy(data+1+SHORT_MAC_LEN, &curRange, 4);
	memcpy(data+5+SHORT_MAC_LEN, &curRXPower, 4);
	copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());

	decaTime deltaTime = decaTime(_replyDelayTimeUS, DW_MICROSECONDS);

	transmit(data, deltaTime, SHORT_MAC_LEN+1+4+4);
}

void rangingClass::transmitRangeFailed(device* myDistantDevice) {
	_globalMac.generateShortMACFrame(data, _currentShortAddress, myDistantDevice->getByteShortAddress());
	data[SHORT_MAC_LEN] = RANGE_FAILED;
	
	copyShortAddress(_lastSentToShortAddress, myDistantDevice->getByteShortAddress());

	transmitNow(data, SHORT_MAC_LEN+1);
}


/* ###########################################################################
 * #### Methods for range computation and corrections  #######################
 * ######################################################################### */


void rangingClass::computeRangeAsymmetric(device* myDistantDevice, decaTime* myTOF) {
	// asymmetric two-way ranging (more computation intense, less error prone)
	decaTime round1 = (myDistantDevice->timePollAckReceived-myDistantDevice->timePollSent).wrap();
	decaTime reply1 = (myDistantDevice->timePollAckSent-myDistantDevice->timePollReceived).wrap();
	decaTime round2 = (myDistantDevice->timeRangeReceived-myDistantDevice->timePollAckSent).wrap();
	decaTime reply2 = (myDistantDevice->timeRangeSent-myDistantDevice->timePollAckReceived).wrap();
	
	myTOF->setTimestamp((round1*round2-reply1*reply2)/(round1+round2+reply1+reply2));
}


/* FOR DEBUGGING*/
/*void rangingClass::visualizeDatas(uint8_t datas[]) {
	MPL_LOGI("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
					datas[0], datas[1], datas[2], datas[3], datas[4], datas[5], datas[6], datas[7], datas[8], datas[9], datas[10], datas[11], datas[12], datas[13], datas[14], datas[15]);
}*/

/* Constructor */
rangingClass::rangingClass() {

};

/* Destructor */
rangingClass::~rangingClass() {

};
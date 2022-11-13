CUDT:
    m_iSYNInterval = 10000
    m_iMSS=1500
    m_bSynSending = true
    m_bSynRecving = true
    m_iFlightFlagSize = 25600
    m_iSndBufSize = 8192
    m_iRcvBufSize = 8192
    m_Linger.l_onoff = 1
    m_Linger.l_linger = 180
    m_iUDPSndBufSize = 65536
    m_iUDPRcvBufSize = m_iRcvBufSize * m_iMSS
    m_bRendezvous = false
    m_iSndTimeOut = -1
    m_iRcvTimeOut = -1
    m_bReuseAddr = true
    m_llMaxBW = -1
    m_ullLingerExpiration = 0
    m_iPktSize = m_iMSS - 28                1472
    m_iPayloadSize = m_iPktSize - CPacket::m_iPktHdrSize    1456
    m_iEXPCount = 1
    m_iBandwidth = 1
    m_iDeliveryRate = 16
    m_iAckSeqNo = 0
    m_ullLastAckTime = 0
    m_StartTime = CTimer::getTime()
    m_llSentTotal = m_llRecvTotal = m_iSndLossTotal = m_iRcvLossTotal = m_iRetransTotal = m_iSentACKTotal = m_iRecvACKTotal = m_iSentNAKTotal = m_iRecvNAKTotal = 0
    m_LastSampleTime = CTimer::getTime()
    m_llTraceSent = m_llTraceRecv = m_iTraceSndLoss = m_iTraceRcvLoss = m_iTraceRetrans = m_iSentACK = m_iRecvACK = m_iSentNAK = m_iRecvNAK = 0
    m_llSndDuration = m_llSndDurationTotal = 0
    m_iRTT = 10 * m_iSYNInterval    10^5    100000
    m_iRTTVar = m_iRTT >> 1         5*10^4  50000
    m_ullCPUFrequency = CTimer::getCPUFrequency()           每微秒钟CPU计数器计数多少次，CPU的频率是多少
    m_ullSYNInt = m_iSYNInterval * m_ullCPUFrequency        10毫秒的频率计数
    m_ullMinNakInt = 300000 * m_ullCPUFrequency             300毫秒的频率计数
    m_ullMinExpInt = 300000 * m_ullCPUFrequency
    m_ullACKInt = m_ullSYNInt                               10毫秒的频率计数
    m_ullNAKInt = m_ullMinNakInt                            300毫秒的频率计数
    m_ullLastRspTime = currtime
    m_ullNextACKTime = currtime + m_ullSYNInt
    m_ullNextNAKTime = currtime + m_ullNAKInt
    m_iPktCount = 0
    m_iLightACKCount = 1
    m_ullTargetTime = 0
    m_ullTimeDiff = 0
    m_iISN = hs->m_iISN                                     random initial sequence number
    m_iPeerISN = hs->m_iISN                                 hs 是 CHandShake。握手传来的消息
    m_iFlowWindowSize = hs->m_iFlightFlagSize
    m_iRcvLastAck = hs->m_iISN
    m_iRcvLastAckAck = hs->m_iISN
    m_iRcvCurrSeqNo = hs->m_iISN - 1
    m_iLastDecSeq = m_iISN - 1
    m_iSndLastAck = m_iISN
    m_iSndLastDataAck = m_iISN
    m_iSndCurrSeqNo = m_iISN - 1
    m_iSndLastAck2 = m_iISN
    m_ullSndLastAck2Time = CTimer::getTime()
    m_ullInterval = (m_pCC->m_dPktSndPeriod * m_ullCPUFrequency)
    m_dCongestionWindow = m_pCC->m_dCWndSize
    m_llSndDurationCounter = CTimer::getTime()              发送持续时间


    m_pQueuedSockets
    m_pAcceptSockets

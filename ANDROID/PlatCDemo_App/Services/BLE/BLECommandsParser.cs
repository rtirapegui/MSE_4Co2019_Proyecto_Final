using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace PlatCDemo_App
{
    class BLECommandsParser
    {
        /*
        * BLE target protocol:
        *
        * (1 byte)    (1 byte)	  (1 byte)		  (pl bytes)	  (1 byte)
        *
        *   STX	+	seqNo	+	payloadLength	+	payload		+	ETX
        *
        */

        /* Protocol control characters */
        private const int STX = 0x02;
        private const int ETX = 0x03;

        /* Protocol states */
        private enum rcvState_t
        {
            CMDS_RCV_IDLE = 0x00
        ,   CMDS_RCV_WAITING_SEQ_NO
        ,   CMDS_RCV_WAITING_EXP_LEN
        ,   CMDS_RCV_WAITING_PAYLOAD_AND_ETX
        }

        private class taskTimeout_t
        {
            public int timeset, timeout;

            public taskTimeout_t(int p1, int p2)
            {
                timeset = p1;
                timeout = p2;
            }
        }

        const int CMD_FRAME_SIZE_MAX = 128;
        const int CMD_FRAME_RCV_TIMEOUT = 100;    // In ms

        // Variables
        private taskTimeout_t g_rcvDataTimeout = new taskTimeout_t(0, 0);
        private rcvState_t g_rcvState = rcvState_t.CMDS_RCV_IDLE;
        private int g_seqNo;
        private int g_expLen;
        private int g_byteCnt;
        private byte[] g_buff = null;

        // Properties
        public delegate void cmdTableDel_t(int seqNo, byte[] payld);

        // Private methods
        private bool IsTimeout(taskTimeout_t data)
        {
            int now = System.Environment.TickCount;

            if ((data.timeset == data.timeout) ||
                 ((data.timeset < data.timeout) && (now < data.timeset || now > data.timeout)) ||
                 ((data.timeset > data.timeout) && (now < data.timeset && now > data.timeout)))
                return true;

            return false;
        }
        private void SetTimeout(ref taskTimeout_t data, int deltaMs)
        {
            data.timeset = System.Environment.TickCount;
            data.timeout = data.timeset + deltaMs;
        }
        private bool GetCompleteCommand(int b)
        {
            bool rst = false;

            if (IsTimeout(g_rcvDataTimeout))
                g_rcvState = rcvState_t.CMDS_RCV_IDLE;

            switch (g_rcvState)
            {
                case rcvState_t.CMDS_RCV_IDLE:
                    {
                        if (STX == b)
                        {
                            g_rcvState = rcvState_t.CMDS_RCV_WAITING_SEQ_NO;
                        }
                    }
                    break;
                case rcvState_t.CMDS_RCV_WAITING_SEQ_NO:
                    {
                        g_rcvState = rcvState_t.CMDS_RCV_WAITING_EXP_LEN;

                        // Save current seq_no
                        g_seqNo = b;
                    }
                    break;
                case rcvState_t.CMDS_RCV_WAITING_EXP_LEN:
                    {
                        if (0 != b)
                        {
                            g_rcvState = rcvState_t.CMDS_RCV_WAITING_PAYLOAD_AND_ETX;

                            // Save current expected length
                            g_expLen = b;
                            g_byteCnt = 0;
                            g_buff = new byte[g_expLen];
                        }
                        else
                        {
                            g_rcvState = rcvState_t.CMDS_RCV_IDLE;
                        }
                    }
                    break;
                case rcvState_t.CMDS_RCV_WAITING_PAYLOAD_AND_ETX:
                    {
                        if (g_byteCnt < g_expLen)
                        {
                            // Save current payload byte
                            g_buff[g_byteCnt] = (byte)b;
                            g_byteCnt++;
                        }
                        else
                        {
                            // Now should come the ETX byte
                            if (ETX == b)
                            {
                                rst = true;
                            }

                            g_rcvState = rcvState_t.CMDS_RCV_IDLE;
                        }
                    }
                    break;
                
                default:
                    {
                        g_rcvState = rcvState_t.CMDS_RCV_IDLE;
                    }
                    break;
            }

            SetTimeout(ref g_rcvDataTimeout, CMD_FRAME_RCV_TIMEOUT);

            return rst;
        }
        private void ProcessCompleteCommand()
        {
            Library.cmdTable(g_seqNo, g_buff);
        }

        // Public methods
        public void ParseIncomingData(byte[] buffer)
        {
            foreach (byte b in buffer)
            {
                if (true == GetCompleteCommand(b))
                {
                    ProcessCompleteCommand();
                }
            }
        }
    }
}
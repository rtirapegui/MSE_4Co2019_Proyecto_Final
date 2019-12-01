using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Newtonsoft.Json;

namespace PlatCDemo_App
{
    class BLECommandsTypes
    {
        /* BLE Target Commands */
        public enum BLETargetCmdId_t
        {
            CMD_ID_ACK = 0          // S --> C
        ,   CMD_ID_DATA             // S --> C
        ,   CMD_ID_GET_DATA         // C --> S
        ,   CMD_ID_CONFIG           // C --> S -- S --> C
        ,   CMD_ID_GET_CONFIG       // C --> S
        ,   CMD_ID_FITNESS          // C --> S -- S --> C
        ,   CMD_ID_SAMPLER_DATA     // S --> C
        ,   CMD_ID_MOTION_DETECTED  // C --> S
        ,   CMD_ID_MAX
        };

        [Serializable]
        public class BLETargetCmd_ack_t
        {
            public int cmd;     // command id
            // Nothing to put in here..
        }
        [Serializable]
        public class BLETargetCmd_data_t
        {
            public int cmd;     // command id
            public uint t;      // timestamp
            public float x;     // accelX
            public float y;     // accelY
            public float z;     // accelZ
            public float u;     // gyroX
            public float v;     // gyroY
            public float w;     // gyroZ
            public float c;     // temperature
            public int b;       // buttonStatus
            public int h;       // motionThreshold
            public int e;       // buzzerEn
            public string n;    // advName
        }
        [Serializable]
        public class BLETargetCmd_getData_t
        {
            public int cmd;    // command id
            // Nothing to put in here..
        }
        [Serializable]
        public class BLETargetCmd_config_t
        {
            public int cmd;     // command id
            public int h;       // motionThreshold
            public int e;       // buzzerEn
            public string n;    // advName
        }
        [Serializable]
        public class BLETargetCmd_getConfig_t
        { 
            public int cmd;     // command id
            // Nothing to put in here..
        }
        [Serializable]
        public class BLETargetCmd_fitness_t
        {
            public int cmd;     // command id
            public int f;       // fitnessFlag
        }
        [Serializable]
        public class BLETargetCmd_samplerData_t
        {
            public int cmd;     // command id
            public uint t;      // timestamp
            public float xm;    // accelX_mean
            public float ym;    // accelY_mean
            public float zm;    // accelZ_mean
            public float xv;    // accelX_var
            public float yv;    // accelY_var
            public float zv;    // accelZ_var
            public float xa;    // accelX_max
            public float ya;    // accelY_max
            public float za;    // accelZ_max
            public float xi;    // accelX_min
            public float yi;    // accelY_min
            public float zi;    // accelZ_min
        }
        [Serializable]
        public class BLETargetCmd_motionDetected_t
        {
            public int cmd;     // command id
            // Nothing to put in here..            
        }

        public static class BLETargetCmd_Helper_cmdId
        {
            public static bool Get(byte[] data, out int cmdId)
            {
                cmdId = (int)BLETargetCmdId_t.CMD_ID_MAX;

                try
                {
                    string jsonStr = Encoding.UTF8.GetString(data);
                    Dictionary<String, Object> dictionary = JsonConvert.DeserializeObject<Dictionary<String, Object>>(jsonStr);

                    if(dictionary.ContainsKey("cmd"))
                    {
                        cmdId = Convert.ToInt32(dictionary["cmd"]);

                        return true;
                    }
                }
                catch(Exception)
                {

                }

                return false;
            }
        }
        public static class BLETargetCmd_Helper_cmdAck
        {
            // Nothing to do here..
        }
        public static class BLETargetCmd_Helper_cmdData
        {
            public static bool Get(byte[] data, out BLETargetCmd_data_t cmdPayload)
            {
                cmdPayload = new BLETargetCmd_data_t();

                try
                {
                    string jsonStr = Encoding.UTF8.GetString(data);
                    cmdPayload = JsonConvert.DeserializeObject<BLETargetCmd_data_t>(jsonStr);

                    return true;
                }
                catch (Exception)
                {

                }

                return false;
            }
        }
        public static class BLETargetCmd_Helper_cmdConfig
        {
            public static bool Get(byte[] data, out BLETargetCmd_config_t cmdPayload)
            {
                cmdPayload = new BLETargetCmd_config_t();

                try
                {
                    string jsonStr = Encoding.UTF8.GetString(data);
                    cmdPayload = JsonConvert.DeserializeObject<BLETargetCmd_config_t>(jsonStr);

                    return true;
                }
                catch (Exception)
                {

                }

                return false;
            }
        }
        public static class BLETargetCmd_Helper_cmdSamplerData
        {
            public static bool Get(byte[] data, out BLETargetCmd_samplerData_t cmdPayload)
            {
                cmdPayload = new BLETargetCmd_samplerData_t();

                try
                {
                    string jsonStr = Encoding.UTF8.GetString(data);
                    cmdPayload = JsonConvert.DeserializeObject<BLETargetCmd_samplerData_t>(jsonStr);

                    return true;
                }
                catch (Exception)
                {

                }

                return false;
            }
        }
    }
}
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

namespace TPMS_App
{
    class BLECommandsTypes
    {
        /* BLE Target Commands */
        public enum BLETargetCmdId_t
        {
            CMD_ID_SENSOR_NOTIFICATION = 0
        ,   CMD_ID_MONITOR_VERSION
        ,   CMD_ID_MAX
        };

        [Serializable]
        public class BLETargetCmd_sensorNotification_t
        {
            public int UniqueId;
            public byte FirmwareVersion;
            public byte DerivativeDescrition;
            public ushort CompensatedPressure;
            public byte AccelerationZStep;
            public ushort CompensatedAccelerationZ;
            public byte CompensatedVoltage;
            public byte CompensatedTemperature;
            public ushort FrameId;
            public byte CRC;
        }

        public static class BLETargetCmd_Helper_SensorNotification
        {
            // Public methods
            public static byte[] ToByteArray(BLETargetCmd_sensorNotification_t obj)
            {
                if (obj != null)
                {
                    BinaryFormatter bf = new BinaryFormatter();
                    using (MemoryStream ms = new MemoryStream())
                    {
                        bf.Serialize(ms, obj);
                        return ms.ToArray();
                    }
                }

                return null;
            }
            public static BLETargetCmd_sensorNotification_t FromByteArray(byte[] data)
            {
                if (data != null)
                {
                    BLETargetCmd_sensorNotification_t _data = new BLETargetCmd_sensorNotification_t()
                    {
                        UniqueId = (((data[3] << 24) & 0x0F) | (data[2] << 16) | (data[1] << 8) | data[0]),
                        FirmwareVersion = data[4],
                        DerivativeDescrition = data[5],
                        CompensatedPressure = (ushort)((data[7] << 8) | data[6]),
                        AccelerationZStep = data[8],
                        CompensatedAccelerationZ = (ushort)((data[10] << 8) | data[9]),
                        CompensatedVoltage = data[11],
                        CompensatedTemperature = data[12],
                        FrameId = (ushort)((data[14] << 8) | data[13]),
                        CRC = data[15],
                    };

                    return _data;
                }

                return null;
            }
        }
    }
}
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace PlatC_Types
{
    public enum PlatC_SensorId_t
    {
        SENSOR_FITNESS = 0
    ,   SENSORS_COUNT_MAX
    };
    public enum PlatC_BuzzerConfig_t
    {
        BUZZER_ENABLE_OFF = 0
    ,   BUZZER_ENABLE_ON
    };
    public enum PlatC_VoiceConfig_t
    {
        VOICE_ENABLE_OFF = 0
    ,   VOICE_ENABLE_ON
    };
    public class PlatC_WearableDescriptor_t
    {
        // Properties
        public int UniqueId { set; get; }
        public int FirmwareVersion { set; get; }
    }
    public class PlatC_ApplicationSettings_t
    {
        public int MotionThreshold;
        public int BuzzerEn;
        public int VoiceEn;
        public int RemainingRepetitionsCount;
        public int RepetitionsCountMin;
        public int RepetitionsCountMax;
        public string AdvertisementName;
    }
}
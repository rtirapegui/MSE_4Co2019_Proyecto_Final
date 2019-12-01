using System;
using Android.App;
using Android.Content;
using PlatC_Types;

namespace PlatCDemo_App
{
    public class IB : IMainActivity 
    {
        /* private values */
        private const int INVALID_IB_VALUE = 0;

        /* ADVERTISE NAME */
        private const string DEFAULT_ADVERTISE_NAME_VALUE = "PlatC";

        private const int ADVERTISE_NAME_SIZE_MAX = 20;

        /* MOTION THRESHOLD */
        private const int DEFAULT_MOTION_THRESHOLD_VALUE = 952;

        private const int MOTION_THRESHOLD_VALUE_MAX = 1020;
        private const int MOTION_THRESHOLD_VALUE_MIN = 40;

        private const string MOTION_UNIT = "mg";

        /* BUZZER */
        private const int DEFAULT_BUZZER_ENABLE_VALUE = 1;

        /* VOICE */
        private const int DEFAULT_VOICE_ENABLE_VALUE = 1;

        /* REPETITIONS COUNT */
        private const int DEFAULT_REPETITIONS_COUNT_VALUE = 15;
        private const int DEFAULT_REPETITIONS_COUNT_VALUE_MIN = 1;
        private const int DEFAULT_REPETITIONS_COUNT_VALUE_MAX = 100;

        /* Interfaces */
        public void IB_getDescriptor(out PlatC_WearableDescriptor_t[] wearableIB)
        {
            object value = 0;
            wearableIB = new PlatC_WearableDescriptor_t[(int)PlatC_SensorId_t.SENSORS_COUNT_MAX]
            {
                new PlatC_WearableDescriptor_t() { UniqueId = INVALID_IB_VALUE, FirmwareVersion = INVALID_IB_VALUE },
            };
                        
            if (IB_getElement(Library.WEARABLE_INFORMATION_BASE, nameof(PlatC_SensorId_t.SENSOR_FITNESS), ref value))
                wearableIB[(int)PlatC_SensorId_t.SENSOR_FITNESS].UniqueId = (int)value;

            if (IB_getElement(Library.WEARABLE_INFORMATION_BASE, nameof(PlatC_SensorId_t.SENSOR_FITNESS), ref value))
                wearableIB[(int)PlatC_SensorId_t.SENSOR_FITNESS].FirmwareVersion = (byte)value;
        }
        public void IB_getSettings(out PlatC_ApplicationSettings_t settingsIB)
        {
            object value = 0;
            settingsIB = new PlatC_ApplicationSettings_t()
            {
                MotionThreshold = (int)DEFAULT_MOTION_THRESHOLD_VALUE,
                BuzzerEn = (int)DEFAULT_BUZZER_ENABLE_VALUE,
                VoiceEn = (int)DEFAULT_VOICE_ENABLE_VALUE,
                RemainingRepetitionsCount = (int)DEFAULT_REPETITIONS_COUNT_VALUE,
                RepetitionsCountMin = (int)DEFAULT_REPETITIONS_COUNT_VALUE_MIN,
                RepetitionsCountMax = (int)DEFAULT_REPETITIONS_COUNT_VALUE_MAX,
                AdvertisementName = (string)DEFAULT_ADVERTISE_NAME_VALUE
            };

            if (IB_getElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.MotionThreshold), ref value))
                settingsIB.MotionThreshold = (int)value;

            if (IB_getElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.BuzzerEn), ref value))
                settingsIB.BuzzerEn = (int)value;

            if (IB_getElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.VoiceEn), ref value))
                settingsIB.VoiceEn = (int)value;

            if (IB_getElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.RemainingRepetitionsCount), ref value))
                settingsIB.RemainingRepetitionsCount = (int)value;

            if (IB_getElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.AdvertisementName), ref value))
                settingsIB.AdvertisementName = (string)value;
        }
        public void IB_setDefaultSettings(out PlatC_ApplicationSettings_t settingsIB)
        {
            object value = 0;
            settingsIB = new PlatC_ApplicationSettings_t()
            {
                MotionThreshold = (int)DEFAULT_MOTION_THRESHOLD_VALUE,
                BuzzerEn = (int)DEFAULT_BUZZER_ENABLE_VALUE,
                VoiceEn = (int)DEFAULT_VOICE_ENABLE_VALUE,
                RemainingRepetitionsCount = (int)DEFAULT_REPETITIONS_COUNT_VALUE,
                RepetitionsCountMin = (int)DEFAULT_REPETITIONS_COUNT_VALUE_MIN,
                RepetitionsCountMax = (int)DEFAULT_REPETITIONS_COUNT_VALUE_MAX,
                AdvertisementName = (string)DEFAULT_ADVERTISE_NAME_VALUE
            };

            IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.MotionThreshold), DEFAULT_MOTION_THRESHOLD_VALUE);
            IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.BuzzerEn), DEFAULT_BUZZER_ENABLE_VALUE);
            IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.VoiceEn), DEFAULT_VOICE_ENABLE_VALUE);
            IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.RemainingRepetitionsCount), DEFAULT_REPETITIONS_COUNT_VALUE);
            IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.AdvertisementName), DEFAULT_ADVERTISE_NAME_VALUE);
        }
        public bool IB_clearElement(string IBName, string elementName)
        {
            try
            {
                var ib = Application.Context.GetSharedPreferences(IBName, FileCreationMode.Private);
                var ibEditor = ib.Edit();

                ibEditor.PutInt(elementName, (int)INVALID_IB_VALUE);

                return ibEditor.Commit();
            }
            catch (Exception)
            {

            }

            return false;
        }
        public bool IB_saveElement(string IBName, string elementName, object value)
        {
            try
            {
                var ib = Application.Context.GetSharedPreferences(IBName, FileCreationMode.Private);
                var ibEditor = ib.Edit();

                if (INVALID_IB_VALUE == (int)value)
                    return false;

                if (value is int)
                {
                    ibEditor.PutInt(elementName, (int)value);
                }
                else if (value is float)
                {
                    ibEditor.PutFloat(elementName, (float)value);
                }
                else if (value is long)
                {
                    ibEditor.PutLong(elementName, (long)value);
                }
                else if (value is string)
                {
                    ibEditor.PutString(elementName, (string)value);
                }
                else
                {
                    return false;
                }

                return ibEditor.Commit();
            }
            catch (Exception)
            {

            }

            return false;
        }
        public bool IB_getElement(string IBName, string elementName, ref object value)
        {
            try
            {
                var ib = Application.Context.GetSharedPreferences(IBName, FileCreationMode.Private);

                if (value is int)
                {
                    value = ib.GetInt(elementName, 0);
                }
                else if (value is float)
                {
                    value = ib.GetFloat(elementName, 0);
                }
                else if (value is long)
                {
                    value = ib.GetLong(elementName, 0);
                }
                else if (value is string)
                {
                    value = ib.GetString(elementName, string.Empty);
                }

                if (INVALID_IB_VALUE == (int)value)
                    return false;

                return true;
            }
            catch (Exception)
            {

            }

            return false;
        }
    }
}
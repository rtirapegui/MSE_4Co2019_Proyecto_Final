using System;
using Android.OS;
using Android.Views;
using Android.Widget;

namespace PlatCDemo_App
{
    public class SettingsFragment : Android.Support.V4.App.Fragment
    {
        private class Seekbar_properties_t
        {
            public int ValueMax;    
            public int ValueMin;    
            public int Value;
        };
        private class SettingsLayoutElement_t
        {
            // Fields
            public bool BuzzerOn = false;
            public bool VoiceOn = false;
            public Seekbar_properties_t RepetitionsCount;
        };

        // Fields
        private const int SEEKBAR_VALUE_MAX = 100;
        private const int SEEKBAR_VALUE_MIN = 1;
        private static SettingsChangedDel_t g_settingsChangedDelegate = null;
        private static SettingsRestoreDefaultValuesDel_t g_settingsRestoreDefaultValuesDelegate = null;
        private static SettingsLayoutElement_t g_settingsLayout = new SettingsLayoutElement_t()
        {
            RepetitionsCount = new Seekbar_properties_t(),
        };
        private static View g_view = null;
        private bool g_seekbarUpdateProgress = true;
        
        // Delegate
        public delegate void SettingsChangedDel_t(bool buzzerOn,
                                                  bool voiceOn, 
                                                  int repetitionsCount);
        public delegate void SettingsRestoreDefaultValuesDel_t();

        // Private methods
        private string SetSeekbarTextValue(Seekbar_properties_t bar)
        {
            if (bar.ValueMin > bar.Value)
                bar.Value = bar.ValueMin;

            if (bar.ValueMax < bar.Value)
                bar.Value = bar.ValueMax;

            return string.Format("{0}", bar.Value);
        }
        private int SetSeekbarMaxValue(Seekbar_properties_t bar)
        {
            return SEEKBAR_VALUE_MAX;
        }
        private int SetSeekbarProgressValue(Seekbar_properties_t bar)
        {
            if (bar.ValueMin > bar.Value)
                bar.Value = bar.ValueMin;

            if (bar.ValueMax < bar.Value)
                bar.Value = bar.ValueMax;

            return (int)(((bar.Value - bar.ValueMin) * SEEKBAR_VALUE_MAX) / (bar.ValueMax - bar.ValueMin));
        }
        private int GetSeekbarProgressValue(Seekbar_properties_t bar, int progress)
        {
            if (bar.ValueMin > bar.Value)
                bar.Value = bar.ValueMin;

            if (bar.ValueMax < bar.Value)
                bar.Value = bar.ValueMax;

            return (((progress * (bar.ValueMax - bar.ValueMin)) / SEEKBAR_VALUE_MAX) + bar.ValueMin);
        }

        // Public methods
        public void RegisterSettingsChangeCb(SettingsChangedDel_t activityCb)
        {
            g_settingsChangedDelegate = activityCb;
        }
        public void RegisterRestoreDefaultValuesSettingsChangeCb(SettingsRestoreDefaultValuesDel_t activityCb)
        {
            g_settingsRestoreDefaultValuesDelegate = activityCb;
        }
        public void UpdateSettingsData(bool buzzerOn,
                                       bool voiceOn,
                                       int repetitionsCountValue, int repetitionsCountMin, int repetitionsCountMax)
        {
            g_settingsLayout.BuzzerOn = buzzerOn;
            g_settingsLayout.VoiceOn = voiceOn;
            g_settingsLayout.RepetitionsCount.ValueMax = repetitionsCountMax;
            g_settingsLayout.RepetitionsCount.ValueMin = repetitionsCountMin;
            g_settingsLayout.RepetitionsCount.Value = repetitionsCountValue;
        }

        // On seekbar changed
        private void OnSeekBarProgressChanged(object sender, SeekBar.ProgressChangedEventArgs e)
        {
            if (true == g_seekbarUpdateProgress)
            {
                SeekBar seekbar = (SeekBar)sender;
                switch (seekbar.Id)
                {
                    case Resource.Id.settings_sb_repetitions_count:
                        {
                            // Invoke activity delegate
                            g_settingsChangedDelegate(g_settingsLayout.BuzzerOn,
                                                      g_settingsLayout.VoiceOn,
                                                      GetSeekbarProgressValue(g_settingsLayout.RepetitionsCount, seekbar.Progress));

                            Activity.RunOnUiThread(() =>
                            {
                                g_view.FindViewById<TextView>(Resource.Id.settings_tv_repetitions_count).Text = SetSeekbarTextValue(g_settingsLayout.RepetitionsCount);
                            });
                        }
                        break;
                }
            }
        }

        // On toggle buttons changed
        private void OnToggleButtonBuzzerCheckedChanged(object sender, CompoundButton.CheckedChangeEventArgs e)
        {
            ToggleButton toggleButton = (ToggleButton) sender;

            // Invoke activity delegate
            g_settingsChangedDelegate(toggleButton.Checked,
                                      g_settingsLayout.VoiceOn,
                                      g_settingsLayout.RepetitionsCount.Value);

            Activity.RunOnUiThread(() => {
                g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_buzzer_sound).Checked = g_settingsLayout.BuzzerOn;
            });
        }
        private void OnToggleButtonVoiceCheckedChanged(object sender, CompoundButton.CheckedChangeEventArgs e)
        {
            ToggleButton toggleButton = (ToggleButton)sender;

            // Invoke activity delegate
            g_settingsChangedDelegate(g_settingsLayout.BuzzerOn,
                                      toggleButton.Checked,
                                      g_settingsLayout.RepetitionsCount.Value);

            Activity.RunOnUiThread(() => {

                g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_voice_sound).Checked = g_settingsLayout.VoiceOn;
            });
        }

        // On reset button changed
        private void OnButtonRestoreDefaultSettingsClick(object sender, EventArgs e)
        {
            // Invoke activity callback
            g_settingsRestoreDefaultValuesDelegate();

            /* Update fragment with default values */
            g_seekbarUpdateProgress = false;

            /* Update buzzer sound */
            g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_buzzer_sound).Checked = g_settingsLayout.BuzzerOn;
            g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_voice_sound).Checked = g_settingsLayout.VoiceOn;

            /* Update voice sound */
            g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_buzzer_sound).Checked = g_settingsLayout.BuzzerOn;
            g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_voice_sound).Checked = g_settingsLayout.VoiceOn;

            /* Update repetitions count */
            g_view.FindViewById<SeekBar>(Resource.Id.settings_sb_repetitions_count).Max = SetSeekbarMaxValue(g_settingsLayout.RepetitionsCount);
            g_view.FindViewById<SeekBar>(Resource.Id.settings_sb_repetitions_count).Progress = SetSeekbarProgressValue(g_settingsLayout.RepetitionsCount);
            g_view.FindViewById<TextView>(Resource.Id.settings_tv_repetitions_count).Text = SetSeekbarTextValue(g_settingsLayout.RepetitionsCount);

            g_seekbarUpdateProgress = true;
        }

        // On create
        public override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
        }

        // On create view
        public override View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
        {
            // Use this to return your custom view for this Fragment
            g_view = inflater.Inflate(Resource.Layout.settings_fragment, container, false);

            /* Enable seekbar progress update */
            g_seekbarUpdateProgress = true;

            /* Configure seekbars */

            // Set seek bars listeners
            SeekBar RepetitionsCount = g_view.FindViewById<SeekBar>(Resource.Id.settings_sb_repetitions_count);

            // Set seekbar properties
            g_view.FindViewById<SeekBar>(Resource.Id.settings_sb_repetitions_count).Max = SetSeekbarMaxValue(g_settingsLayout.RepetitionsCount);
            g_view.FindViewById<SeekBar>(Resource.Id.settings_sb_repetitions_count).Progress = SetSeekbarProgressValue(g_settingsLayout.RepetitionsCount);
            g_view.FindViewById<TextView>(Resource.Id.settings_tv_repetitions_count).Text = SetSeekbarTextValue(g_settingsLayout.RepetitionsCount);

            // Set seekbars listeners
            RepetitionsCount.ProgressChanged += (s, e) =>
            {
                OnSeekBarProgressChanged(s, e);
            };

            /* Configure Toggle button */
            ToggleButton tbBuzzerSound = g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_buzzer_sound);
            ToggleButton tbVoiceSound = g_view.FindViewById<ToggleButton>(Resource.Id.settings_tb_voice_sound);

            tbBuzzerSound.Checked = g_settingsLayout.BuzzerOn;
            tbVoiceSound.Checked = g_settingsLayout.VoiceOn;

            // Set toggle button listener
            tbBuzzerSound.CheckedChange += (s, e) =>
            {
                OnToggleButtonBuzzerCheckedChanged(s, e);
            };
            tbVoiceSound.CheckedChange += (s, e) =>
            {
                OnToggleButtonVoiceCheckedChanged(s, e);
            };
            
            /* Configure restore default values button */
            g_view.FindViewById<Button>(Resource.Id.settings_btn_restore_default_values).Click += OnButtonRestoreDefaultSettingsClick;

            return g_view;
        }
    }
}
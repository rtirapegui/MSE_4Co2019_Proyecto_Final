using System;
using System.Timers;
using Android.OS;
using Android.Views;
using Android.Widget;
using PlatC_Types;

namespace PlatCDemo_App
{
    public class HomeFragment : Android.Support.V4.App.Fragment
    {
        private const string LABEL_EXERCISE_MODE = "Abdominales restantes";
        private const string LABEL_CALIBRATION_MODE = "Tiempo restante (segundos)";
        private const int CALIBRATION_INTERVAL = 60;

        private class HomeLayoutElement_t
        {
            // Fields
            public bool ModeOn = false;
            public bool StatusOn = false;
            public int repetitionsCount;
            public string variableString;
        };

        // Fields
        private static HomeLayoutElement_t g_homeLayout = new HomeLayoutElement_t()
        {
            ModeOn = false,
            StatusOn = false
        };
        private static HomeBtnChangedDel_t g_btnChangedDelegate = null;
        private static HomeCalibrationEndDel_t g_calibrationEndDelegate = null;
        private static View g_view = null;
        private static Timer g_timer = null;
        private static int g_timerCount;

        // Delegates
        public delegate void HomeBtnChangedDel_t(bool modeOn, bool statusOn);
        public delegate void HomeCalibrationEndDel_t();

        // Private methods
        private void Timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            g_timerCount--;

            if (0 == g_timerCount)
            {
                g_timer.Stop();
                g_timerCount = CALIBRATION_INTERVAL;

                g_calibrationEndDelegate();
            }

            Activity.RunOnUiThread(() => {
                g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches_counter).Text = g_timerCount.ToString();
            });
        }
        private void OnToggleButtonModeCheckedChanged(object sender, CompoundButton.CheckedChangeEventArgs e)
        {
            ToggleButton toggleButton = (ToggleButton)sender;

            if(false == g_homeLayout.StatusOn)
            {
                // Invoke activity delegate
                g_btnChangedDelegate(toggleButton.Checked,
                                      g_homeLayout.StatusOn);
            }
        }
        private void OnToggleButtonStatusCheckedChanged(object sender, CompoundButton.CheckedChangeEventArgs e)
        {
            ToggleButton toggleButton = (ToggleButton)sender;

            // Invoke activity delegate
            g_btnChangedDelegate(g_homeLayout.ModeOn,
                                  toggleButton.Checked);
        }

        // Public methods
        public void SetButtons(bool modeOn, bool statusOn)
        {
            g_homeLayout.ModeOn = modeOn;
            g_homeLayout.StatusOn = statusOn;

            if((true == g_homeLayout.StatusOn) && (false == g_homeLayout.ModeOn))
            {
                // Mode == calibration
                g_timerCount = CALIBRATION_INTERVAL;
                g_timer.Start();
            }
            else
            {
                // Mode == exercice
                g_timer.Stop();
            }

            Activity.RunOnUiThread(() => {
                g_view.FindViewById<ToggleButton>(Resource.Id.home_tb_mode).Enabled = !g_homeLayout.StatusOn;
                g_view.FindViewById<ToggleButton>(Resource.Id.home_tb_status).Checked = g_homeLayout.StatusOn;
                g_view.FindViewById<ToggleButton>(Resource.Id.home_tb_mode).Checked = g_homeLayout.ModeOn;

                if(true == g_homeLayout.ModeOn)
                {
                    g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches).Text = LABEL_EXERCISE_MODE;
                    g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches_counter).Text = g_homeLayout.repetitionsCount.ToString();
                }
                else
                {
                    g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches).Text = LABEL_CALIBRATION_MODE;
                    g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches_counter).Text = g_timerCount.ToString();
                }
            });
        }
        public void RegisterHomeBtnChangeCb(HomeBtnChangedDel_t activityCb)
        {
            g_btnChangedDelegate = activityCb;
        }
        public void RegisterHomeCalibrationCb(HomeCalibrationEndDel_t activityCb)
        {
            g_calibrationEndDelegate = activityCb;
        }
        public void UpdateRepetitionsCount(int repetitionsCount)
        {
            try
            {
                g_homeLayout.repetitionsCount = repetitionsCount;

                Activity.RunOnUiThread(() => {
                    g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches_counter).Text = g_homeLayout.repetitionsCount.ToString();
                });
            }
            catch(Exception)
            {

            }
        }
        public void UpdateVariableString(string variableString)
        {
            try
            {
                g_homeLayout.variableString = variableString;

                Activity.RunOnUiThread(() => {
                    g_view.FindViewById<ToggleButton>(Resource.Id.home_tv_rem_crunches_variable).Text = g_homeLayout.variableString;
                });
            }
            catch (Exception)
            {

            }
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
            g_view = inflater.Inflate(Resource.Layout.home_fragment, container, false);

            /* Configure Toggle button */
            ToggleButton tbMode = g_view.FindViewById<ToggleButton>(Resource.Id.home_tb_mode);
            ToggleButton tbStatus = g_view.FindViewById<ToggleButton>(Resource.Id.home_tb_status);

            tbMode.Checked = g_homeLayout.ModeOn;
            tbStatus.Checked = g_homeLayout.StatusOn;

            // Set toggle button listener
            tbMode.CheckedChange += (s, e) =>
            {
                OnToggleButtonModeCheckedChanged(s, e);
            };
            tbStatus.CheckedChange += (s, e) =>
            {
                OnToggleButtonStatusCheckedChanged(s, e);
            };

            /* Set objects data */
            g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches).Text = LABEL_CALIBRATION_MODE;
            g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches_counter).Text = g_timerCount.ToString();
            g_view.FindViewById<TextView>(Resource.Id.home_tv_rem_crunches_variable).Text = g_homeLayout.variableString;

            /* Instanciate count down timer */
            g_timer = new Timer();
            g_timer.Interval = 1000;
            g_timer.Elapsed += Timer_Elapsed;

            return g_view;
        }
    }
}
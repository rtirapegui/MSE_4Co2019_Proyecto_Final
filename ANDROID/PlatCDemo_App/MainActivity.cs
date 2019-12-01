using System;
using Android.App;
using Android.OS;
using Android.Support.Design.Widget;
using Android.Support.V4.View;
using Android.Support.V4.Widget;
using Android.Support.V7.App;
using Android.Views;
using nexus.protocols.ble;
using Android.Content;
using Android.Content.PM;
using Android.Locations;
using PlatC_Types;
using static PlatCDemo_App.BLECommandsTypes;
using Plugin.TextToSpeech;
using System.Collections.Generic;
using Newtonsoft.Json;
using Utilities.Math;
using System.Linq;

namespace Utilities.Math
{
    /// <summary>
    /// Various math related functions
    /// </summary>
    public static class MathHelper
    {
        #region Public Static Functions
        /// <summary>
        /// Gets the mean value from a list
        /// </summary>
        /// <param name="Values">The list of values</param>
        /// <returns>The mean/average of the list</returns>
        public static double Mean(System.Collections.Generic.List<int> Values)
        {
            if (Values.Count == 0)
                return 0.0;
            double ReturnValue = 0.0;
            for (int x = 0; x < Values.Count; ++x)
            {
                ReturnValue += Values[x];
            }
            return ReturnValue / (double)Values.Count;
        }

        /// <summary>
        /// Gets the mean value from a list
        /// </summary>
        /// <param name="Values">The list of values</param>
        /// <returns>The mean/average of the list</returns>
        public static double Mean(System.Collections.Generic.List<double> Values)
        {
            if (Values.Count == 0)
                return 0.0;
            double ReturnValue = 0.0;
            for (int x = 0; x < Values.Count; ++x)
            {
                ReturnValue += Values[x];
            }
            return ReturnValue / (double)Values.Count;
        }


        /// <summary>
        /// Calculates the variance of a list of values
        /// </summary>
        /// <param name="Values">List of values</param>
        /// <returns>The variance</returns>
        public static double Variance(System.Collections.Generic.List<double> Values)
        {
            if (Values == null || Values.Count == 0)
                return 0;
            double MeanValue = Mean(Values);
            double Sum = 0;
            for (int x = 0; x < Values.Count; ++x)
            {
                Sum += System.Math.Pow(Values[x] - MeanValue, 2);
            }
            return Sum / (double)Values.Count;
        }

        /// <summary>
        /// Gets the standard deviation
        /// </summary>
        /// <param name="Values">List of values</param>
        /// <returns>The standard deviation</returns>
        public static double StandardDeviation(System.Collections.Generic.List<double> Values)
        {
            return System.Math.Sqrt(Variance(Values));
        }

        public static double Mean(double[] x)
        {
            double sum = 0;
            for (int i = 0; i < x.Length; i++)
                sum += x[i];

            return sum / x.Length;
        }
        public static double[] Autocorrelation(double[] x)
        {
            double mean = Mean(x);

            double[] autocorrelation = new double[x.Length / 2];
            for (int t = 0; t < autocorrelation.Length; t++)
            {
                double n = 0; // Numerator
                double d = 0; // Denominator

                for (int i = 0; i < (x.Length - t); i++)
                {
                    double xim = x[i] - mean;
                    n += xim * (x[(i + t) % x.Length] - mean);
                    d += xim * xim;
                }

                autocorrelation[t] = n / d;
            }

            return autocorrelation;
        }

        enum CurveStateValley
        {
            GoingDown = 0, EqGoingDown = 1, NotGoingDown = 2
        }
        public static List<int> GetValleys(List<double> a)
        {
            var res = new List<int>();
            if (a.Count < 2)
            {
                return res;
            }
            int lastEq = 0;
            CurveStateValley s = CurveStateValley.NotGoingDown;
            for (var i = 1; i != a.Count; i++)
            {
                switch (System.Math.Sign(a[i] - a[i - 1]))
                {
                    case -1:
                        s = CurveStateValley.GoingDown;
                        break;
                    case 0:
                        if (s == CurveStateValley.GoingDown)
                        {
                            lastEq = i;
                        }
                        s = (s == CurveStateValley.NotGoingDown) ? CurveStateValley.NotGoingDown : CurveStateValley.EqGoingDown;
                        break;
                    case 1:
                        if (s == CurveStateValley.GoingDown)
                        {
                            res.Add(i - 1);
                        }
                        else if (s == CurveStateValley.EqGoingDown)
                        {
                            res.Add((lastEq + i - 1) / 2);
                        }
                        s = CurveStateValley.NotGoingDown;
                        break;
                }
            }
            return res;
        }
        enum CurveStateHill
        {
            GoingUp = 0, EqGoingUp = 1, NotGoingUp = 2
        }
        public static List<int> GetHills(List<double> a)
        {
            var res = new List<int>();
            if (a.Count < 2)
            {
                return res;
            }
            int lastEq = 0;
            CurveStateHill s = CurveStateHill.NotGoingUp;
            for (var i = 1; i != a.Count; i++)
            {
                switch (System.Math.Sign(a[i] - a[i - 1]))
                {
                    case 1:
                        s = CurveStateHill.GoingUp;
                        break;
                    case 0:
                        if (s == CurveStateHill.GoingUp)
                        {
                            lastEq = i;
                        }
                        s = (s == CurveStateHill.NotGoingUp) ? CurveStateHill.NotGoingUp : CurveStateHill.EqGoingUp;
                        break;
                    case -1:
                        if (s == CurveStateHill.GoingUp)
                        {
                            res.Add(i - 1);
                        }
                        else if (s == CurveStateHill.EqGoingUp)
                        {
                            res.Add((lastEq + i - 1) / 2);
                        }
                        s = CurveStateHill.NotGoingUp;
                        break;
                }
            }
            return res;
        }
        #endregion
    }
}

namespace PlatCDemo_App
{
    [BroadcastReceiver(Enabled = true, Exported = false)]
    public class OnLocationAdapterStateChange : BroadcastReceiver
    {
        public override void OnReceive(Context context, Intent intent)
        {
            if (intent.Action == LocationManager.ProvidersChangedAction)
            {
                LocationManager lm = (LocationManager)context.GetSystemService("location");

                if (!lm.IsProviderEnabled(LocationManager.GpsProvider) || !lm.IsProviderEnabled(LocationManager.NetworkProvider))
                    Library.CloseApplication((Activity)context);
            }
        }
    }

    [Activity(Icon = "@mipmap/wearable", Label = "@string/app_name", Theme = "@style/AppTheme.NoActionBar", MainLauncher = true, ScreenOrientation = ScreenOrientation.Portrait)]
    public class MainActivity : AppCompatActivity, NavigationView.IOnNavigationItemSelectedListener, IChartsFragment
    {
        enum Fragment_id_t
        {
            HOME_FRAGMENT_ID = 0,
            CHART_FRAGMENT_ID,
            SETTINGS_FRAGMENT_ID,
            INFO_FRAGMENT_ID,
            FRAGMENT_ID_MAX
        };

        private class RunTimeData
        {
            public bool Mode;                      // True == Exercice; False = Calibration;
            public bool Status;                    // True == Started; False = Stopped;
            public int RemainingRepetitionsCount;            // True == ; False = ;
            public int ActiveAxis;                 // 0 == x, 1 == y, 2 == z
            public double AutocorrThresholdMax;    // 0 == x, 1 == y, 2 == z
            public double AutocorrThresholdMin;    // 0 == x, 1 == y, 2 == z
        }

        private class CmdRsp_fitness
        {
            public bool waitingFlag;
            public bool modeOn;
            public bool statusOn;
        }

        private const string HOME_FRAGMENT_TITLE = "Fitness";
        private const string CHART_FRAGMENT_TITLE = "Gráficos";
        private const string SETTINGS_FRAGMENT_TITLE = "Ajustes";
        private const string INFO_FRAGMENT_TITLE = "Acerca de PlatC";

        private const string HOME_FRAGMENT_SUBTITLE_CONNECTED = "Conectando a PlatC";
        private const string HOME_FRAGMENT_SUBTITLE_CONNECTING = "Conectado a PlatC";
        private const string HOME_FRAGMENT_SUBTITLE_DISCONNECTING = "Desconectando de PlatC";
        private const string HOME_FRAGMENT_SUBTITLE_DISCONNECTED = "Desconectado de PlatC";

        private const string HOME_VARIABLE_STRING_INITIAL = "";

        /* Working objects */
        private readonly Handler g_uiThreadHandler = new Handler(Looper.MainLooper);

        // BLE Adapter
        private BLE g_BLEAdapter;

        // Wearable Information Base
        private PlatC_WearableDescriptor_t[] g_wearableDescriptorIB;

        // Settings Information Base
        private PlatC_ApplicationSettings_t g_wearableSettingsIB;

        // Acceleration variables
        List<double> g_ax_mean = new List<double>(), g_ay_mean = new List<double>(), g_az_mean = new List<double>();
        List<double> g_ax_var = new List<double>(), g_ay_var = new List<double>(), g_az_var = new List<double>();
        List<double> g_ax_max = new List<double>(), g_ay_max = new List<double>(), g_az_max = new List<double>();
        List<double> g_ax_min = new List<double>(), g_ay_min = new List<double>(), g_az_min = new List<double>();

        List<double> g_ax_meanCal = new List<double>(), g_ay_meanCal = new List<double>(), g_az_meanCal = new List<double>();
        List<double> g_ax_varCal = new List<double>(), g_ay_varCal = new List<double>(), g_az_varCal = new List<double>();

        // Fragments
        private int g_activeFragmentId;
        private HomeFragment g_homeFragment = new HomeFragment();
        private ChartsFragment g_chartFragment = new ChartsFragment();
        private SettingsFragment g_settingsFragment = new SettingsFragment();
        private InfoFragment g_infoFragment = new InfoFragment();
        private RunTimeData g_runtimeData = new RunTimeData()
        {
            ActiveAxis = 1,
            AutocorrThresholdMax = 0.3,
            AutocorrThresholdMin = -0.3,
        };
        private CmdRsp_fitness g_waitingCmdRspFitness = null;

        // Main menu layout
        private IMenu g_menuMain = null;
        private Android.Support.V7.Widget.Toolbar g_toolbar = null;
        private string[] g_toolbarSubtitle = new string[(int)Fragment_id_t.FRAGMENT_ID_MAX];    // One string subtitle for each application fragment

        private void CalculateThresholds(double[] meanSerie)
        {
            double[] meanAutocorrSerie = MathHelper.Autocorrelation(meanSerie);
            List<double> meanAutocorrSerieList = new List<double>(meanAutocorrSerie);
            List<double> meanAutocorrMinSerieList = new List<double>();
            List<double> meanAutocorrSerieMaxList = new List<double>();
            meanAutocorrSerieList.RemoveAt(0);

            /* Promediate 3 hills and valleys */
            for (int i = 0; i < 3;i++)
            {
                meanAutocorrMinSerieList.Add(meanAutocorrSerieList.Min());
                meanAutocorrSerieList.Remove(meanAutocorrSerieList.Min());
                meanAutocorrSerieMaxList.Add(meanAutocorrSerieList.Max());
                meanAutocorrSerieList.Remove(meanAutocorrSerieList.Max());
            }

            g_runtimeData.AutocorrThresholdMin = MathHelper.Mean(meanAutocorrMinSerieList);
            g_runtimeData.AutocorrThresholdMax = MathHelper.Mean(meanAutocorrSerieMaxList);
        }
        private int CalculateRepetitions(double[] meanAutocorrSerie)
        {
            if (0 < meanAutocorrSerie.Count())
            {
                /* 1. Count the number of hills and valleys above and below thresholds */
                /* 2. Find pairs of values. Each pair represent one repetition done. */
                /* 3. Return the number of repetitions in the serie. */
                int aboveThresholdCnt = 0;
                int belowThresholdCnt = 0;
                int repetitionsCnt = 0;

                List<double> _meanAutocorrSerieList = new List<double>(meanAutocorrSerie);
                _meanAutocorrSerieList.Remove(1);

                foreach (double sample in _meanAutocorrSerieList)
                {
                    if (g_runtimeData.AutocorrThresholdMin >= sample)
                        belowThresholdCnt++;

                    if (g_runtimeData.AutocorrThresholdMax <= sample)
                        aboveThresholdCnt++;
                }

                while ((0 < aboveThresholdCnt) && (0 < belowThresholdCnt))
                {
                    repetitionsCnt++;
                    aboveThresholdCnt--;
                    belowThresholdCnt--;
                }

                return repetitionsCnt;
            }

            return 0;
        }
        private void ClearCalibrationData()
        {
            g_ax_meanCal.Clear();
            g_ay_meanCal.Clear();
            g_az_meanCal.Clear();
            g_ax_varCal.Clear();
            g_ay_varCal.Clear();
            g_az_varCal.Clear();
        }

        /* Activity callbacks */
        public void OnBLEConnectionStateChange(ConnectionState state)
        {
            // Set Home Fragment subtitle
            switch (state)
            {
                case ConnectionState.Connecting: g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_CONNECTED; g_toolbarSubtitle[(int)Fragment_id_t.CHART_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_CONNECTED; break;
                case ConnectionState.Connected: g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_CONNECTING; g_toolbarSubtitle[(int)Fragment_id_t.CHART_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_CONNECTING; break;
                case ConnectionState.Disconnecting: g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_DISCONNECTING; g_toolbarSubtitle[(int)Fragment_id_t.CHART_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_DISCONNECTING;  break;
                case ConnectionState.Disconnected: g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_DISCONNECTED; g_toolbarSubtitle[(int)Fragment_id_t.CHART_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_DISCONNECTED;  break;
            }

            // Update Home Fragment subtitle if is the current active fragment
            if (Resource.Id.home == g_activeFragmentId)
            {
                g_uiThreadHandler.Post(() => {
                    g_toolbar.Subtitle = g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID];
                });
            }
        }
        public void OnSettingsChanged(bool buzzerOn,
                                      bool voiceOn,
                                      int repetitionsCount)
        {
            // Save data into settings information base and update local information
            int buzzerSound = (int)(true == buzzerOn ? PlatC_BuzzerConfig_t.BUZZER_ENABLE_ON : PlatC_BuzzerConfig_t.BUZZER_ENABLE_OFF);
            if (buzzerSound != g_wearableSettingsIB.BuzzerEn)
            {
                Library.IbObj.IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.BuzzerEn), buzzerSound);
                g_wearableSettingsIB.BuzzerEn = buzzerSound;
            }

            int voiceSound = (int)(true == voiceOn ? PlatC_VoiceConfig_t.VOICE_ENABLE_ON : PlatC_VoiceConfig_t.VOICE_ENABLE_OFF);
            if (voiceSound != g_wearableSettingsIB.VoiceEn)
            {
                Library.IbObj.IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.VoiceEn), voiceSound);
                g_wearableSettingsIB.VoiceEn = voiceSound;
            }

            if (repetitionsCount != g_wearableSettingsIB.RemainingRepetitionsCount)
            {
                Library.IbObj.IB_saveElement(Library.SETTINGS_INFORMATION_BASE, nameof(PlatC_ApplicationSettings_t.RemainingRepetitionsCount), repetitionsCount);
                g_wearableSettingsIB.RemainingRepetitionsCount = repetitionsCount;
            }

            // Update Settings fragment data
            g_settingsFragment.UpdateSettingsData(buzzerOn,
                                                  voiceOn,
                                                  repetitionsCount,
                                                  g_wearableSettingsIB.RepetitionsCountMin,
                                                  g_wearableSettingsIB.RepetitionsCountMax);

            // Update home values
            UpdateHomeFragmentDataValues(repetitionsCount);

            // Enviar el comando config de BLE

        }
        public void OnRestoreDefaultSettings()
        {
            Library.IbObj.IB_setDefaultSettings(out g_wearableSettingsIB);

            // Update Settings fragment data
            g_settingsFragment.UpdateSettingsData(Convert.ToBoolean(g_wearableSettingsIB.BuzzerEn),
                                                  Convert.ToBoolean(g_wearableSettingsIB.VoiceEn),
                                                  g_wearableSettingsIB.RemainingRepetitionsCount,
                                                  g_wearableSettingsIB.RepetitionsCountMin,
                                                  g_wearableSettingsIB.RepetitionsCountMax);

            // Update home values
            UpdateHomeFragmentDataValues(g_wearableSettingsIB.RemainingRepetitionsCount);

            // Enviar el comando config de BLE

        }
        public void OnHomeButtonClicked(bool modeOn, bool statusOn)
        {
            ConnectionState connState = g_BLEAdapter.GetConnectionState();

            if (ConnectionState.Connected == connState)
            {
                /* Send fitness command with status flag */
                for(int i = 0; i < 4;i++)
                    SendCommand_Fitness(statusOn);

                ClearCalibrationData();

                g_waitingCmdRspFitness = new CmdRsp_fitness()
                {
                    waitingFlag = true,
                    modeOn = modeOn,
                    statusOn = statusOn
                };

                /* Update fragment buttons with current values */
                g_homeFragment.SetButtons(g_runtimeData.Mode, g_runtimeData.Status);
            }
            else
            {
                g_runtimeData.Mode = modeOn;
                g_runtimeData.Status = false;

                /* Update fragment buttons */
                g_homeFragment.SetButtons(g_runtimeData.Mode, g_runtimeData.Status);
            }
        }
        public void OnHomeCalibrationEnd()
        {
            /* Send fitness command with status flag */
            ConnectionState connState = g_BLEAdapter.GetConnectionState();

            if (ConnectionState.Connected == connState)
            {
                for (int i = 0; i < 10; i++)
                    SendCommand_Fitness(false);
            }

            g_waitingCmdRspFitness = new CmdRsp_fitness()
            {
                waitingFlag = true,
                modeOn = false,
                statusOn = false
            };

            /* Decide which axis presents maximum variance */
            double xVar = MathHelper.Variance(g_ax_varCal);
            double yVar = MathHelper.Variance(g_ay_varCal);
            double zVar = MathHelper.Variance(g_az_varCal);

            if (xVar > yVar && xVar > zVar)
            {
                g_runtimeData.ActiveAxis = 0;
                CalculateThresholds(g_ax_meanCal.ToArray());
            }
            else if (yVar > xVar && yVar > zVar)
            {
                g_runtimeData.ActiveAxis = 1;
                CalculateThresholds(g_ay_meanCal.ToArray());
            }
            else
            {
                g_runtimeData.ActiveAxis = 2;
                CalculateThresholds(g_az_meanCal.ToArray());
            }

            ClearCalibrationData();
        }

        /* Operative functions */
        private const int AUTOCORRELATION_WINDOWS_SIZE = 10;

        private async void OnSamplerData()
        {
            /* Process incoming sampler data and update home fragment */
            if (true == g_runtimeData.Mode)
            {
                /* Mode == exercise */
                double[] meanSerie = null;

                /* Calculate autocorrelation of sample data frame */
                switch (g_runtimeData.ActiveAxis)
                {
                    case 0:
                        {
                            if(AUTOCORRELATION_WINDOWS_SIZE < g_ax_meanCal.Count())
                            {
                                meanSerie = g_ax_meanCal.TakeLast(AUTOCORRELATION_WINDOWS_SIZE).ToArray();
                            }
                            else
                            {
                                meanSerie = g_ax_meanCal.ToArray();
                            }
                        } break;
                    case 1:
                        {
                            if (AUTOCORRELATION_WINDOWS_SIZE < g_ay_meanCal.Count())
                            {
                                meanSerie = g_ay_meanCal.TakeLast(AUTOCORRELATION_WINDOWS_SIZE).ToArray();
                            }
                            else
                            {
                                meanSerie = g_ay_meanCal.ToArray();
                            }
                        } break;
                    case 2:
                        {
                            if (AUTOCORRELATION_WINDOWS_SIZE < g_az_meanCal.Count())
                            {
                                meanSerie = g_az_meanCal.TakeLast(AUTOCORRELATION_WINDOWS_SIZE).ToArray();
                            }
                            else
                            {
                                meanSerie = g_az_meanCal.ToArray();
                            }
                        } break;
                    default: return;
                }

                if (null == meanSerie)
                    return;

                List<double> autocorrSerieList = MathHelper.Autocorrelation(meanSerie).ToList();
                autocorrSerieList.Remove(1);

                /* Calculate repetitions count during exercise routine */
                int repetitionCnt = CalculateRepetitions(autocorrSerieList.ToArray());

                if (0 < repetitionCnt)
                    ClearCalibrationData();

                g_runtimeData.RemainingRepetitionsCount -= repetitionCnt;

                if (0 > g_runtimeData.RemainingRepetitionsCount)
                    g_runtimeData.RemainingRepetitionsCount = 0;

                /* Update repetitions count */
                g_homeFragment.UpdateRepetitionsCount(g_runtimeData.RemainingRepetitionsCount);

                /* Check if sound advice is enabled */
                if(0 < repetitionCnt)
                {
                    /* Check if voice guidance is enabled */
                    if (0 != g_wearableSettingsIB.VoiceEn)
                    {
                        if(10 < g_runtimeData.RemainingRepetitionsCount)
                        {
                            await CrossTextToSpeech.Current.Speak(string.Format("Faltan {0} abdominales!", g_runtimeData.RemainingRepetitionsCount));
                        }
                        else if (5 < g_runtimeData.RemainingRepetitionsCount)
                        {
                            await CrossTextToSpeech.Current.Speak(string.Format("Dale que faltan {0}", g_runtimeData.RemainingRepetitionsCount));
                        }
                        else if (3 < g_runtimeData.RemainingRepetitionsCount)
                        {
                            await CrossTextToSpeech.Current.Speak(string.Format("Dale genio ya lo tenes faltan {0}", g_runtimeData.RemainingRepetitionsCount));
                        }
                        else if (2 < g_runtimeData.RemainingRepetitionsCount)
                        {
                            await CrossTextToSpeech.Current.Speak(string.Format("Un esfuerzo mas, dale vos podes"));
                        }
                        else if (0 == g_runtimeData.RemainingRepetitionsCount)
                        {
                            await CrossTextToSpeech.Current.Speak(string.Format("Terminamos!"));
                        }
                    }
                }
            }
        }
        private void UpdateHomeFragmentDataValues(int repetitionCount)
        {
            if (false == g_runtimeData.Status)
            {
                g_homeFragment.UpdateRepetitionsCount(repetitionCount);
            }
        }
        private void EditMainMenuOptions()
        {
            if (null != g_menuMain)
            {
                g_uiThreadHandler.Post(() =>
                {
                    if (Resource.Id.home == g_activeFragmentId)
                    {
                        // Check BLE connection state and show desired menu element
                        ConnectionState connState = g_BLEAdapter.GetConnectionState();

                        if ((ConnectionState.Connected == connState) || (ConnectionState.Connecting == connState))
                        {
                            g_menuMain.FindItem(Resource.Id.home_action_BLEConnect).SetVisible(false);
                            g_menuMain.FindItem(Resource.Id.home_action_BLEDisconnect).SetVisible(true);
                        }
                        else
                        {
                            g_menuMain.FindItem(Resource.Id.home_action_BLEConnect).SetVisible(true);
                            g_menuMain.FindItem(Resource.Id.home_action_BLEDisconnect).SetVisible(false);
                        }
                    }
                    else
                    {
                        // Hide all options menu
                        g_menuMain.FindItem(Resource.Id.home_action_BLEConnect).SetVisible(false);
                        g_menuMain.FindItem(Resource.Id.home_action_BLEDisconnect).SetVisible(false);
                    }
                });
            }
        }
        private void SetInitialFragmentSubtitles()
        {
            // Home fragment
            g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_DISCONNECTED;

            // Chart fragment
            g_toolbarSubtitle[(int)Fragment_id_t.CHART_FRAGMENT_ID] = HOME_FRAGMENT_SUBTITLE_DISCONNECTED;
        }

        /**************/
        /* Interfaces */
        /**************/
        /* Get acceleration samples */
        public bool GetAccelXMeanSamples(out double[] ax_mean)
        {
            ax_mean = new double[0];

            if (0 < g_ax_mean.Count)
            {
                ax_mean = g_ax_mean.ToArray();
                g_ax_mean.Clear();
                return true;
            }

            return false;
        }
        /* Get acceleration samples */
        public bool GetAccelYMeanSamples(out double[] ay_mean)
        {
            ay_mean = new double[0];

            if (0 < g_ay_mean.Count)
            {
                ay_mean = g_ay_mean.ToArray();
                g_ay_mean.Clear();
                return true;
            }

            return false;
        }
        /* Get acceleration samples */
        public bool GetAccelZMeanSamples(out double[] az_mean)
        {
            az_mean = new double[0];

            if (0 < g_az_mean.Count)
            {
                az_mean = g_az_mean.ToArray();
                g_az_mean.Clear();
                return true;
            }

            return false;
        }
        /* Get acceleration samples */
        public bool GetAccelXVarianceSamples(out double[] ax_var)
        {
            ax_var = new double[0];

            if (0 < g_ax_var.Count)
            {
                ax_var = g_ax_var.ToArray();
                g_ax_var.Clear();
                return true;
            }

            return false;
        }
        /* Get acceleration samples */
        public bool GetAccelYVarianceSamples(out double[] ay_var)
        {
            ay_var = new double[0];

            if (0 < g_ay_var.Count)
            {
                ay_var = g_ay_var.ToArray();
                g_ay_var.Clear();
                return true;
            }

            return false;
        }
        /* Get acceleration samples */
        public bool GetAccelZVarianceSamples(out double[] az_var)
        {
            az_var = new double[0];

            if (0 < g_az_var.Count)
            {
                az_var = g_az_var.ToArray();
                g_az_var.Clear();
                return true;
            }

            return false;
        }

        /************************************/
        /*      BLE Outgoing Commands       */
        /************************************/
        private async void SendCommand_Fitness(bool flag)
        {
            try
            {
                BLETargetCmd_fitness_t _cmdData = new BLETargetCmd_fitness_t()
                {
                    cmd = (int)BLETargetCmdId_t.CMD_ID_FITNESS,
                    f = Convert.ToInt32(flag)
                };

                string jsonstr = JsonConvert.SerializeObject(_cmdData);
                byte[] cmdData = System.Text.Encoding.ASCII.GetBytes(jsonstr);

                byte[] cmd = new byte[1 + 1 + 1 + (byte)cmdData.Length + 1];    // STX   +   seqNo +    payloadLength   +   payload +   ETX

                cmd[0] = 0x02;
                cmd[1] = 0x01;
                cmd[2] = (byte)cmdData.Length;
                System.Buffer.BlockCopy(cmdData, 0, cmd, 3, cmdData.Length);
                cmd[3 + cmdData.Length] = 0x03;

                await g_BLEAdapter.BLE_WriteTargetCharacteristic(cmd);
            }
            catch (Exception)
            {

            }
        }
        private async void SendCommand_MotionDetected()
        {
            try
            {
                BLETargetCmd_motionDetected_t _cmdData = new BLETargetCmd_motionDetected_t()
                {
                    cmd = (int)BLETargetCmdId_t.CMD_ID_MOTION_DETECTED,
                };

                string jsonstr = JsonConvert.SerializeObject(_cmdData);
                byte[] cmdData = System.Text.Encoding.ASCII.GetBytes(jsonstr);

                byte[] cmd = new byte[1 + 1 + 1 + (byte)cmdData.Length + 1];    // STX   +   seqNo +    payloadLength   +   payload +   ETX

                cmd[0] = 0x02;
                cmd[1] = 0x01;
                cmd[2] = (byte)cmdData.Length;
                System.Buffer.BlockCopy(cmdData, 0, cmd, 3, cmdData.Length);
                cmd[3 + cmdData.Length] = 0x03;

                await g_BLEAdapter.BLE_WriteTargetCharacteristic(cmd);
            }
            catch (Exception)
            {

            }
        }

        /************************************/
        /* BLE Incoming Commands Processors */
        /************************************/
        private void ProcessAckCmd(int seqNo, byte[] payld)
        {
            if((null != g_waitingCmdRspFitness) && (true == g_waitingCmdRspFitness.waitingFlag))
            {
                /* Update runtime data */
                g_runtimeData.Mode = g_waitingCmdRspFitness.modeOn;
                g_runtimeData.Status = g_waitingCmdRspFitness.statusOn;
                g_runtimeData.RemainingRepetitionsCount = g_wearableSettingsIB.RemainingRepetitionsCount;

                /* Update home fragment */
                g_homeFragment.UpdateRepetitionsCount(g_runtimeData.RemainingRepetitionsCount);
                g_homeFragment.SetButtons(g_runtimeData.Mode, g_runtimeData.Status);

                g_waitingCmdRspFitness = null;
            }
        }
        private async void ProcessDataCmd(int seqNo, byte[] payld)
        {
            BLETargetCmd_data_t data;

            if(true == BLETargetCmd_Helper_cmdData.Get(payld, out data))
            {
                // Nothing to do here..                
            }
        }
        private void ProcessConfigCmd(int seqNo, byte[] payld)
        {
            BLETargetCmd_config_t data;

            if (true == BLETargetCmd_Helper_cmdConfig.Get(payld, out data))
            {
                // Nothing to do here..
            }
        }
        private void ProcessSamplerDataCmd(int seqNo, byte[] payld)
        {
            BLETargetCmd_samplerData_t data;

            if (true == BLETargetCmd_Helper_cmdSamplerData.Get(payld, out data))
            {
                /* Populate lists */ 
                g_ax_mean.Add(data.xm); g_ax_meanCal.Add(data.xm);
                g_ay_mean.Add(data.ym); g_ay_meanCal.Add(data.ym);
                g_az_mean.Add(data.zm); g_az_meanCal.Add(data.zm);
                g_ax_var.Add(data.xv);  g_ax_varCal.Add(data.xv);
                g_ay_var.Add(data.yv);  g_ay_varCal.Add(data.yv);
                g_az_var.Add(data.zv);  g_az_varCal.Add(data.zv);
                g_ax_max.Add(data.xa);
                g_ay_max.Add(data.ya);
                g_az_max.Add(data.za);
                g_ax_min.Add(data.xi);
                g_ay_min.Add(data.yi);
                g_az_min.Add(data.zi);

                /* Signal incoming sampler data */
                OnSamplerData();
            }
        }
        private void OnBLEIncomingCommand(int seqNo, byte[] payld)
        {
            /* Get command id from payload */
            try
            {
                int cmdId;

                if (true == BLETargetCmd_Helper_cmdId.Get(payld, out cmdId))
                {
                    switch (cmdId)
                    {
                        case (int)BLETargetCmdId_t.CMD_ID_ACK:
                            {
                                ProcessAckCmd(seqNo, payld);
                            }
                            break;
                        case (int)BLETargetCmdId_t.CMD_ID_DATA:
                            {
                                ProcessDataCmd(seqNo, payld);
                            }
                            break;
                        case (int)BLETargetCmdId_t.CMD_ID_CONFIG:
                            {
                                ProcessConfigCmd(seqNo, payld);
                            }
                            break;
                        case (int)BLETargetCmdId_t.CMD_ID_SAMPLER_DATA:
                            {
                                ProcessSamplerDataCmd(seqNo, payld);
                            }
                            break;
                            // case (int)BLETargetCmdId_t.CMD_ID_GET_DATA:
                            // case (int)BLETargetCmdId_t.CMD_ID_GET_CONFIG:
                            // case (int)BLETargetCmdId_t.CMD_ID_FITNESS:
                            // case (int)BLETargetCmdId_t.CMD_ID_MOTION_DETECTED:
                        default:
                            {
                                // Nothing to do here..
                            }
                            break;
                    }
                }
            }
            catch (Exception)
            {

            }
        }

        /* Activity callbacks */
        protected sealed override void OnActivityResult(Int32 requestCode, Result resultCode, Intent data)
        {
            // Invoke BLE Adapter Activity Result
            BluetoothLowEnergyAdapter.OnActivityResult(requestCode, resultCode, data);
        }

        /* Layout callbacks */
        private void DisplayView(int id)
        {
            try
            {
                if (id == Resource.Id.home)
                {
                    g_homeFragment = new HomeFragment();
                    Android.Support.V4.App.FragmentTransaction ft = SupportFragmentManager.BeginTransaction();
                    ft.Replace(Resource.Id.main_frame, g_homeFragment);
                    ft.Commit();

                    // Set active fragment id
                    g_activeFragmentId = Resource.Id.home;

                    g_uiThreadHandler.Post(() =>
                    {
                        // Set fragment title
                        g_toolbar.Title = HOME_FRAGMENT_TITLE;

                        // Set fragment subtitle
                        g_toolbar.Subtitle = g_toolbarSubtitle[(int)Fragment_id_t.HOME_FRAGMENT_ID];
                    });
                }
                else if (id == Resource.Id.chart)
                {
                    g_chartFragment = new ChartsFragment();
                    Android.Support.V4.App.FragmentTransaction ft = SupportFragmentManager.BeginTransaction();
                    ft.Replace(Resource.Id.main_frame, g_chartFragment);
                    ft.Commit();

                    // Set active fragment id
                    g_activeFragmentId = Resource.Id.chart;

                    g_uiThreadHandler.Post(() =>
                    {
                        // Set fragment title
                        g_toolbar.Title = CHART_FRAGMENT_TITLE;

                        // Set fragment subtitle
                        g_toolbar.Subtitle = g_toolbarSubtitle[(int)Fragment_id_t.CHART_FRAGMENT_ID];
                    });
                }
                else if (id == Resource.Id.settings)
                {
                    g_settingsFragment = new SettingsFragment();
                    Android.Support.V4.App.FragmentTransaction ft = SupportFragmentManager.BeginTransaction();
                    ft.Replace(Resource.Id.main_frame, g_settingsFragment);
                    ft.Commit();

                    g_uiThreadHandler.Post(() =>
                    {
                        // Set active fragment id
                        g_activeFragmentId = Resource.Id.settings;

                        // Set fragment title
                        g_toolbar.Title = SETTINGS_FRAGMENT_TITLE;

                        // Set fragment subtitle
                        g_toolbar.Subtitle = g_toolbarSubtitle[(int)Fragment_id_t.SETTINGS_FRAGMENT_ID];
                    });
                }
                else if (id == Resource.Id.information)
                {
                    g_infoFragment = new InfoFragment();
                    Android.Support.V4.App.FragmentTransaction ft = SupportFragmentManager.BeginTransaction();
                    ft.Replace(Resource.Id.main_frame, g_infoFragment);
                    ft.Commit();

                    g_uiThreadHandler.Post(() =>
                    {
                        // Set active fragment id
                        g_activeFragmentId = Resource.Id.information;

                        // Set fragment title
                        g_toolbar.Title = INFO_FRAGMENT_TITLE;

                        // Set fragment subtitle
                    });
                }

                // Adapt Main Menu visile options based on active fragment
                EditMainMenuOptions();

                DrawerLayout drawer = FindViewById<DrawerLayout>(Resource.Id.drawer_layout);
                drawer.CloseDrawer(GravityCompat.Start);
            }
            catch (Exception)
            {

            }
        }

        /*************/
        /* On Create */
        /*************/
        protected override async void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);

            // Set your trial or purchased license key here!
            // 
            // If you don't have a trial key you can get one from www.scichart.com/licensing-scichart-android
            // 
            // Use code similar to the following: 
            // 
            // var licensingContract = @"Get your license from www.scichart.com/licensing-scichart-android";
            // SciChart.Charting.Visuals.SciChartSurface.SetRuntimeLicenseKey(licensingContract);
            SciChart.Charting.Visuals.SciChartSurface.SetRuntimeLicenseKey(@"<LicenseContract> 
             <Customer>rtirapeg@itba.edu.ar</Customer>
             <OrderId>Trial</OrderId>
             <LicenseCount>1</LicenseCount>
             <IsTrialLicense>true</IsTrialLicense>
             <SupportExpires>11/24/2019 00:00:00</SupportExpires>
             <ProductCode>SC-ANDROID-2D-ENTERPRISE-SRC</ProductCode>
             <KeyCode>0d5b620878d1c19f7e81c1c51e80a5e8ba96f4a0cd0655bf316cad19af2fea6257a1ad6ff9bc8c8634a3a0e11e7340322e39f4433847b88822fd5103511a26954ba4c01db9dbb0fa326c9407d3ca24049dfc3b43b86d37caabcb5805fe182821188943da913914f7cb72353c0df3d14b6375a045f569a380f5f2bb04eb8331a3c256a6ec35fb8130e4a8b2174c1ed440089d1aad23a435f084979feab5e9e0924ffbe65778b8588a473c48bb14a9</KeyCode>
             </LicenseContract>");
            SetContentView(Resource.Layout.activity_main);

            // Set toolbar
            g_toolbar = FindViewById<Android.Support.V7.Widget.Toolbar>(Resource.Id.toolbar);
            SetSupportActionBar(g_toolbar);

            // Set drawer
            DrawerLayout drawer = FindViewById<DrawerLayout>(Resource.Id.drawer_layout);
            ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawer, g_toolbar, Resource.String.navigation_drawer_open, Resource.String.navigation_drawer_close);
            drawer.AddDrawerListener(toggle);
            toggle.SyncState();

            // Set navigation view
            NavigationView navigationView = FindViewById<NavigationView>(Resource.Id.nav_view);
            navigationView.SetNavigationItemSelectedListener(this);

            // Set activity handler
            Library.SetActivity(this);

            // Initialize Bluetooth Low Energy Adapter
            g_BLEAdapter = new BLE();

            if (false == await g_BLEAdapter.Initialize(this, ApplicationContext, OnBLEIncomingCommand))
                Library.CloseApplication(this);

            // Retrieve wearable descriptor IB
            Library.IbObj.IB_getDescriptor(out g_wearableDescriptorIB);

            // Retrieve wearable setting IB
            Library.IbObj.IB_getSettings(out g_wearableSettingsIB);

            // Update Settings fragment data
            g_settingsFragment.UpdateSettingsData(Convert.ToBoolean(g_wearableSettingsIB.BuzzerEn), 
                                                  Convert.ToBoolean(g_wearableSettingsIB.VoiceEn),
                                                  g_wearableSettingsIB.RemainingRepetitionsCount,
                                                  g_wearableSettingsIB.RepetitionsCountMin,
                                                  g_wearableSettingsIB.RepetitionsCountMax);

            // Set home fragment initial data
            g_homeFragment.UpdateRepetitionsCount(g_wearableSettingsIB.RemainingRepetitionsCount);
            g_homeFragment.UpdateVariableString(HOME_VARIABLE_STRING_INITIAL);

            // Register for BLE connection state changes
            g_BLEAdapter.RegisterBLEConnectionStateChangeCb(OnBLEConnectionStateChange);

            // Register for application settings changes
            g_settingsFragment.RegisterSettingsChangeCb(OnSettingsChanged);
            g_settingsFragment.RegisterRestoreDefaultValuesSettingsChangeCb(OnRestoreDefaultSettings);

            // Register for application devices button clicked
            g_homeFragment.RegisterHomeBtnChangeCb(OnHomeButtonClicked);
            g_homeFragment.RegisterHomeCalibrationCb(OnHomeCalibrationEnd);

            // Set initial fragment subtitles
            SetInitialFragmentSubtitles();

            // Show Home Fragment display view
            DisplayView(Resource.Id.home);
        }

        /**************/
        /*  On Pause  */
        /**************/

        /**************/
        /* On Resume  */
        /**************/

        /**************/
        /* On Destroy */
        /**************/
        public override void OnBackPressed()
        {
            DrawerLayout drawer = FindViewById<DrawerLayout>(Resource.Id.drawer_layout);
            if (drawer.IsDrawerOpen(GravityCompat.Start))
            {
                drawer.CloseDrawer(GravityCompat.Start);
            }
            else
            {
                MoveTaskToBack(true);
            }
        }
        public override bool OnCreateOptionsMenu(IMenu menu)
        {
            g_menuMain = menu;
            MenuInflater.Inflate(Resource.Menu.menu_main, menu);
            return true;
        }
        public override bool OnPrepareOptionsMenu(IMenu menu)
        {
            // Adapt Main Menu visile options based on active fragment
            EditMainMenuOptions();
            return base.OnPrepareOptionsMenu(menu);
        }
        public override bool OnOptionsItemSelected(IMenuItem item)
        {
            int id = item.ItemId;
            if (id == Resource.Id.home_action_BLEConnect)
            {
                // Find and connect BLE Monitor
                g_BLEAdapter.BLE_ScanAndConnect(Library.BLE_DEVICE_NAME_TARGET);

                return true;
            }
            else if (id == Resource.Id.home_action_BLEDisconnect)
            {
                // Disconnect BLE Monitor
                g_BLEAdapter.BLE_Disconnect();
            }

            return base.OnOptionsItemSelected(item);
        }
        public bool OnNavigationItemSelected(IMenuItem item)
        {
            DisplayView(item.ItemId);
            return true;
        }
    }
}


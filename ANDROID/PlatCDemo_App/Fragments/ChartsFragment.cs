using Android.OS;
using Android.Views;
using Java.Lang;
using SciChart.Charting.Model;
using SciChart.Charting.Model.DataSeries;
using SciChart.Charting.Modifiers;
using SciChart.Charting.Visuals;
using SciChart.Charting.Visuals.Annotations;
using SciChart.Charting.Visuals.Axes;
using SciChart.Charting.Visuals.RenderableSeries;
using SciChart.Drawing.Common;
using System.Timers;
using SciChart.Core.Model;
using Android.Widget;

namespace PlatCDemo_App
{
    public class ChartsFragment : Android.Support.V4.App.Fragment
    {
        /* Private fields */
        private const int TIMER_INTERVAL_ACCEL_X = 1234;
        private const int TIMER_INTERVAL_ACCEL_Y = 1567;
        private const int TIMER_INTERVAL_ACCEL_Z = 1890;
        private static readonly int POINT_COUNT_MAX = CalculateMaxPointCountToDisplay();

        /* Private objects */
        private IChartsFragment g_IChart;

        /* Private variables */
        private volatile bool g_isRunning = false;
        private readonly object g_syncRoot = new object();
        private readonly Handler g_uiThreadHandler = new Handler(Looper.MainLooper);
        private TextView g_tvAccelX, g_tvAccelY, g_tvAccelZ;
        private Timer g_timerAccelX, g_timerAccelY, g_timerAccelZ;

        private int g_accelX_XValue = 0, g_accelY_XValue = 0, g_accelZ_XValue = 0;

        private readonly XyDataSeries<int, float> g_accelX_MeanSeries = new XyDataSeries<int, float>();
        private readonly XyDataSeries<int, float> g_accelX_VarSeries = new XyDataSeries<int, float>();
        private readonly XyDataSeries<int, float> g_accelY_MeanSeries = new XyDataSeries<int, float>();
        private readonly XyDataSeries<int, float> g_accelY_VarSeries = new XyDataSeries<int, float>();
        private readonly XyDataSeries<int, float> g_accelZ_MeanSeries = new XyDataSeries<int, float>();
        private readonly XyDataSeries<int, float> g_accelZ_VarSeries = new XyDataSeries<int, float>();

        public SciChartSurface g_accelX_Surface => View.FindViewById<SciChartSurface>(Resource.Id.ax_surface);
        public SciChartSurface g_accelY_Surface => View.FindViewById<SciChartSurface>(Resource.Id.ay_surface);
        public SciChartSurface g_accelZ_Surface => View.FindViewById<SciChartSurface>(Resource.Id.az_surface);

        /* Private static methods */
        private static int CalculateMaxPointCountToDisplay()
        {
            const int oneMlnPointsRequirement = 8 + 16 + 4 + 8;
            var memorySize = GetMaxMemorySize() - 40;
            var maxPointCount = memorySize / oneMlnPointsRequirement * 1000000;

            return (int) System.Math.Round(maxPointCount / 3);
        }
        private static double GetMaxMemorySize()
        {
            return Runtime.GetRuntime().MaxMemory() / 1024.0 / 1024.0;
        }

        /* Private methods */
        private int GetPointsCount()
        {
            return g_accelX_MeanSeries.Count + g_accelY_MeanSeries.Count + g_accelZ_MeanSeries.Count + g_accelX_VarSeries.Count + g_accelY_VarSeries.Count + g_accelZ_VarSeries.Count;
        }

        private void AccelX_doAppendLoop(double[] dataMean, double[] dataVar)
        {
            int len = dataMean.Length;

            IntegerValues XValues = new IntegerValues(len);
            FloatValues YValues_mean = new FloatValues(len);
            FloatValues YValues_var = new FloatValues(len);

            using (g_accelX_Surface.SuspendUpdates())
            {
                g_accelX_XValue++;

                XValues.Clear();
                YValues_mean.Clear();
                YValues_var.Clear();

                for (var i = 0;i < dataMean.Length;i++)
                {
                    XValues.Add(g_accelX_XValue);
                    YValues_mean.Add((float) dataMean[i]);
                    YValues_var.Add((float)dataVar[i]);
                }

                g_accelX_MeanSeries.Append(XValues, YValues_mean);
                g_accelX_VarSeries.Append(XValues, YValues_var);
            }
        }
        private void AccelY_doAppendLoop(double[] dataMean, double[] dataVar)
        {
            int len = dataMean.Length;

            IntegerValues XValues = new IntegerValues(len);
            FloatValues YValues_mean = new FloatValues(len);
            FloatValues YValues_var = new FloatValues(len);

            using (g_accelY_Surface.SuspendUpdates())
            {
                g_accelY_XValue++;

                XValues.Clear();
                YValues_mean.Clear();
                YValues_var.Clear();

                for (var i = 0; i < dataMean.Length; i++)
                {
                    XValues.Add(g_accelY_XValue);
                    YValues_mean.Add((float)dataMean[i]);
                    YValues_var.Add((float)dataVar[i]);
                }

                g_accelY_MeanSeries.Append(XValues, YValues_mean);
                g_accelY_VarSeries.Append(XValues, YValues_var);
            }
        }
        private void AccelZ_doAppendLoop(double[] dataMean, double[] dataVar)
        {
            int len = dataMean.Length;

            IntegerValues XValues = new IntegerValues(len);
            FloatValues YValues_mean = new FloatValues(len);
            FloatValues YValues_var = new FloatValues(len);

            using (g_accelZ_Surface.SuspendUpdates())
            {
                g_accelZ_XValue++;

                XValues.Clear();
                YValues_mean.Clear();
                YValues_var.Clear();

                for (var i = 0; i < dataMean.Length; i++)
                {
                    XValues.Add(g_accelZ_XValue);
                    YValues_mean.Add((float)dataMean[i]);
                    YValues_var.Add((float)dataVar[i]);
                }

                g_accelZ_MeanSeries.Append(XValues, YValues_mean);
                g_accelZ_VarSeries.Append(XValues, YValues_var);
            }
        }
        private void OnTick_AccelX(object sender, ElapsedEventArgs e)
        {
            lock(g_syncRoot)
            {
                if(!g_isRunning) return;

                /* Plot X axis means */
                if (true == g_IChart.GetAccelXMeanSamples(out double[] ax_mean))
                {
                    if (true == g_IChart.GetAccelXVarianceSamples(out double[] ax_var))
                    {
                        if (GetPointsCount() < POINT_COUNT_MAX)
                        {
                            AccelX_doAppendLoop(ax_mean, ax_var);
                        }
                        else
                        {
                            Pause();
                        }
                    }
                }
            }
        }
        private void OnTick_AccelY(object sender, ElapsedEventArgs e)
        {
            lock (g_syncRoot)
            {
                if (!g_isRunning) return;

                /* Plot Y axis */
                if (true == g_IChart.GetAccelYMeanSamples(out double[] ay_mean))
                {
                    if (true == g_IChart.GetAccelYVarianceSamples(out double[] ay_var))
                    {
                        if (GetPointsCount() < POINT_COUNT_MAX)
                        {
                            AccelY_doAppendLoop(ay_mean, ay_var);
                        }
                        else
                        {
                            Pause();
                        }
                    }
                }
            }
        }
        private void OnTick_AccelZ(object sender, ElapsedEventArgs e)
        {
            lock (g_syncRoot)
            {
                if (!g_isRunning) return;

                /* Plot Z axis */
                if (true == g_IChart.GetAccelZMeanSamples(out double[] az_mean))
                {
                    if (true == g_IChart.GetAccelZVarianceSamples(out double[] az_var))
                    {
                        if (GetPointsCount() < POINT_COUNT_MAX)
                        {
                            AccelZ_doAppendLoop(az_mean, az_var);
                        }
                        else
                        {
                            Pause();
                        }
                    }
                }
            }
        }
        private void Pause()
        {
            if (!g_isRunning) return;

            g_isRunning = false;
            g_timerAccelX.Stop();
            g_timerAccelX.Elapsed -= OnTick_AccelX;
            g_timerAccelX = null;

            g_timerAccelY.Stop();
            g_timerAccelY.Elapsed -= OnTick_AccelY;
            g_timerAccelY = null;

            g_timerAccelZ.Stop();
            g_timerAccelZ.Elapsed -= OnTick_AccelZ;
            g_timerAccelZ = null;
        }
        private void Start()
        {
            if (g_isRunning) return;

            g_isRunning = true;
            g_timerAccelX = new Timer(TIMER_INTERVAL_ACCEL_X);
            g_timerAccelY = new Timer(TIMER_INTERVAL_ACCEL_Y);
            g_timerAccelZ = new Timer(TIMER_INTERVAL_ACCEL_Z);

            g_timerAccelX.Elapsed += OnTick_AccelX;
            g_timerAccelX.AutoReset = true;
            g_timerAccelX.Start();

            g_timerAccelY.Elapsed += OnTick_AccelY;
            g_timerAccelY.AutoReset = true;
            g_timerAccelY.Start();

            g_timerAccelZ.Elapsed += OnTick_AccelZ;
            g_timerAccelZ.AutoReset = true;
            g_timerAccelZ.Start();
        }
        private void Reset()
        {
            if (g_isRunning)
                Pause();

            using (g_accelX_Surface.SuspendUpdates())
            {
                g_accelX_MeanSeries.Clear();
                g_accelX_VarSeries.Clear();
            }
            using (g_accelY_Surface.SuspendUpdates())
            {
                g_accelY_MeanSeries.Clear();
                g_accelY_VarSeries.Clear();
            }
            using (g_accelZ_Surface.SuspendUpdates())
            {
                g_accelZ_MeanSeries.Clear();
                g_accelZ_VarSeries.Clear();
            }

            g_accelX_XValue = 0;
            g_accelY_XValue = 0;
            g_accelZ_XValue = 0;
        }
        private void InitFragment()
        {
            g_IChart = Library.GetISensorsFragmentObject();

            View.FindViewById<Button>(Resource.Id.start).Click += (sender, args) => Start();
            View.FindViewById<Button>(Resource.Id.pause).Click += (sender, args) => Pause();
            View.FindViewById<Button>(Resource.Id.reset).Click += (sender, args) => Reset();

            var xAxis = new NumericAxis(Activity) { AutoRange = AutoRange.Always };
            var yAxis = new NumericAxis(Activity) { AutoRange = AutoRange.Always };

            var rsX_mean = new FastLineRenderableSeries { DataSeries = g_accelX_MeanSeries, StrokeStyle = new SolidPenStyle(0xFF4083B7, 2f.ToDip(Activity))};
            var rsX_var = new FastLineRenderableSeries { DataSeries = g_accelX_VarSeries, StrokeStyle = new SolidPenStyle(0xFFFFA500, 2f.ToDip(Activity))};
            var rsY_mean = new FastLineRenderableSeries { DataSeries = g_accelY_MeanSeries, StrokeStyle = new SolidPenStyle(0xFF4083B7, 2f.ToDip(Activity))};
            var rsY_var = new FastLineRenderableSeries { DataSeries = g_accelY_VarSeries, StrokeStyle = new SolidPenStyle(0xFFFFA500, 2f.ToDip(Activity))};
            var rsZ_mean = new FastLineRenderableSeries { DataSeries = g_accelZ_MeanSeries, StrokeStyle = new SolidPenStyle(0xFF4083B7, 2f.ToDip(Activity))};
            var rsZ_var = new FastLineRenderableSeries { DataSeries = g_accelZ_VarSeries, StrokeStyle = new SolidPenStyle(0xFFFFA500, 2f.ToDip(Activity))};
            
            /*
            g_tvAccelX = new TextView(Activity);
            g_tvAccelX.SetPadding(20, 20, 20, 20);
            var annotationX = new CustomAnnotation(Activity)
            {
                CoordinateMode = AnnotationCoordinateMode.Relative,
                ZIndex = -1,
                X1Value = 0,
                Y1Value = 0,
            };
            annotationX.SetContentView(g_tvAccelX);

            g_tvAccelY = new TextView(Activity);
            g_tvAccelY.SetPadding(20, 20, 20, 20);
            var annotationY = new CustomAnnotation(Activity)
            {
                CoordinateMode = AnnotationCoordinateMode.Relative,
                ZIndex = -1,
                X1Value = 0,
                Y1Value = 0,
            };
            annotationY.SetContentView(g_tvAccelY);

            g_tvAccelZ = new TextView(Activity);
            g_tvAccelZ.SetPadding(20, 20, 20, 20);
            var annotationZ = new CustomAnnotation(Activity)
            {
                CoordinateMode = AnnotationCoordinateMode.Relative,
                ZIndex = -1,
                X1Value = 0,
                Y1Value = 0,
            };
            annotationZ.SetContentView(g_tvAccelZ);
            */

            using (g_accelX_Surface.SuspendUpdates())
            {
                g_accelX_Surface.XAxes.Add(xAxis);
                g_accelX_Surface.YAxes.Add(yAxis);
                g_accelX_Surface.RenderableSeries.Add(rsX_mean);
                g_accelX_Surface.RenderableSeries.Add(rsX_var);
                /*
                g_accelX_Surface.ChartModifiers = new ChartModifierCollection
                {
                    new ZoomPanModifier(),
                    new PinchZoomModifier(),
                    new ZoomExtentsModifier(),
                };
                //g_accelX_Surface.Annotations.Add(annotationX);
                */
            }
            using (g_accelY_Surface.SuspendUpdates())
            {
                g_accelY_Surface.XAxes.Add(xAxis);
                g_accelY_Surface.YAxes.Add(yAxis);
                g_accelY_Surface.RenderableSeries.Add(rsY_mean);
                g_accelY_Surface.RenderableSeries.Add(rsY_var);
                /*
                g_accelY_Surface.ChartModifiers = new ChartModifierCollection
                {
                    new ZoomPanModifier(),
                    new PinchZoomModifier(),
                    new ZoomExtentsModifier(),
                };
                //g_accelY_Surface.Annotations.Add(annotationY);
                */
            }
            using (g_accelZ_Surface.SuspendUpdates())
            {
                g_accelZ_Surface.XAxes.Add(xAxis);
                g_accelZ_Surface.YAxes.Add(yAxis);
                g_accelZ_Surface.RenderableSeries.Add(rsZ_mean);
                g_accelZ_Surface.RenderableSeries.Add(rsZ_var);
                /*
                g_accelZ_Surface.ChartModifiers = new ChartModifierCollection
                {
                    new ZoomPanModifier(),
                    new PinchZoomModifier(),
                    new ZoomExtentsModifier(),
                };
                //g_accelZ_Surface.Annotations.Add(annotationZ);
                */
            }
            Reset();
        }

        /* Public methods */
        public override View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
        {
            return inflater.Inflate(Resource.Layout.charts_fragment, null);
        }
        public override void OnStart()
        {
            base.OnStart();

            InitFragment();
        }
        public override void OnDestroyView()
        {
            base.OnDestroyView();

            Reset();
        }
    }
}

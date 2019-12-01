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
    public interface IChartsFragment
    {
        /* Get acceleration samples */
        bool GetAccelXMeanSamples(out double[] ax_mean);

        /* Get acceleration samples */
        bool GetAccelYMeanSamples(out double[] ay_mean);

        /* Get acceleration samples */
        bool GetAccelZMeanSamples(out double[] az_mean);

        /* Get acceleration samples */
        bool GetAccelXVarianceSamples(out double[] ax_var);

        /* Get acceleration samples */
        bool GetAccelYVarianceSamples(out double[] ay_var);

        /* Get acceleration samples */
        bool GetAccelZVarianceSamples(out double[] az_var);
    }
}
using System;
using Android.Content;
using Android.Util;

namespace PlatCDemo_App
{
    public static class NumericExtensions
    {
        public static float ToDip(this float value, Context context)
        {
            return TypedValue.ApplyDimension(ComplexUnitType.Dip, value, context.Resources.DisplayMetrics);
        }

        public static int ToDip(this int value, Context context)
        {
            var dipValue = TypedValue.ApplyDimension(ComplexUnitType.Dip, value, context.Resources.DisplayMetrics);
            return (int) Math.Round(dipValue);
        }
    }
}
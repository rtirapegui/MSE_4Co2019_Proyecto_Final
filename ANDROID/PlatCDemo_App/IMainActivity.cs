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
using PlatC_Types;

namespace PlatCDemo_App
{
    public interface IMainActivity
    {
        /* Read wearable IB */
        void IB_getDescriptor(out PlatC_WearableDescriptor_t[] wearableIB);

        /* Read wearable settings IB */
        void IB_getSettings(out PlatC_ApplicationSettings_t settingsIB);
    }
}
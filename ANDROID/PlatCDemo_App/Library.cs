using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using nexus.protocols.ble;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using nexus.protocols.ble.scan;
using nexus.protocols.ble.gatt.adopted;

namespace PlatCDemo_App
{
    static class Library
    {
        /* Public objects */
        public static IB IbObj = new IB();
        public static MainActivity g_activity;

        /* Methods */
        public static void SetActivity(MainActivity context)
        {
            g_activity = context;
        }
        public static void CloseApplication(Activity context)
        {
            var activity = context;

            activity.FinishAffinity();
        }

        /* Factory|singleton pattern: match interface implementer
         * */
        public static IChartsFragment Obj_ISensorsFragment;

        /* Public methods */
        public static IChartsFragment GetISensorsFragmentObject()
        {
            Obj_ISensorsFragment = g_activity;

            return Obj_ISensorsFragment;
        }

        /* Instanciate below all static object needed by the application */

        /* Bluetooth adapter related definitions */
        public const string BLE_DEVICE_NAME_TARGET = "PlatC";
        public const string BLE_DEVICE_SERVICE_UUID_TARGET = "0000FFE0-0000-1000-8000-00805F9B34FB";
        public const string BLE_DEVICE_CHARACTERISTIC_UUID_TARGET = "0000FFE1-0000-1000-8000-00805F9B34FB";
        public const string WEARABLE_INFORMATION_BASE = "PLATC_WEARABLE_IB";
        public const string SETTINGS_INFORMATION_BASE = "PLATC_SETTINGS_IB";
        
        /* Commands related objets */
        public static BLECommandsParser.cmdTableDel_t cmdTable = null;
    }
}
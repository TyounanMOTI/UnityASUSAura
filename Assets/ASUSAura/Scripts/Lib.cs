using System.Runtime.InteropServices;
using System;

namespace ASUSAura {
    public class Lib {
        const int TRUE = 1;
        const int FALSE = 0;
        const int listSize = 32;
        const int LEDListSize = 2048;
        const string dllPath = "UnityASUSAura";

        public delegate void DebugLogDelegate([In, MarshalAs(UnmanagedType.LPWStr)]string message);

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern void set_debug_log_func(DebugLogDelegate func);
        public static void SetDebugLogFunc(DebugLogDelegate func) {
            set_debug_log_func(func);
        }

        [DllImport(dllPath)]
        private static extern int is_available();
        public static bool IsAvailable() {
            return is_available() == TRUE;
        }

        [DllImport(dllPath)]
        private static extern int query_switch_state();
        public static bool QuerySwitchState() {
            return query_switch_state() == TRUE;
        }

        [DllImport(dllPath)]
        private static extern void set_switch_state(int state);
        public static void SetSwitchState(bool state) {
            set_switch_state((state) ? TRUE : FALSE);
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern int get_device_name_list(IntPtr[] names, int max_list_length);
        public static string[] GetDeviceNameList() {
            var namePointers = new IntPtr[listSize];
            var length = get_device_name_list(namePointers, listSize);
            var names = new string[length];
            for (var i = 0; i < length; i++) {
                names[i] = Marshal.PtrToStringBSTR(namePointers[i]);
                Marshal.FreeBSTR(namePointers[i]);
            }
            return names;
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern int get_effect_list(IntPtr[] ids, IntPtr[] names, int max_list_length);
        public static void GetEffectList(out string[] ids, out string[] names) {
            var idPointers = new IntPtr[listSize];
            var namePointers = new IntPtr[listSize];
            var length = get_effect_list(idPointers, namePointers, listSize);
            ids = new string[length];
            names = new string[length];
            for (var i = 0; i < length; i++) {
                ids[i] = Marshal.PtrToStringBSTR(idPointers[i]);
                names[i] = Marshal.PtrToStringBSTR(namePointers[i]);
                Marshal.FreeBSTR(idPointers[i]);
                Marshal.FreeBSTR(namePointers[i]);
            }
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern int get_LED_list(IntPtr[] ids, IntPtr[] names, int max_list_length);
        public static void GetLEDList(out string[] ids, out string[] names) {
            var idPointers = new IntPtr[LEDListSize];
            var namePointers = new IntPtr[LEDListSize];
            var length = get_LED_list(idPointers, namePointers, LEDListSize);
            ids = new string[length];
            names = new string[length];
            for (var i = 0; i < length; i++) {
                ids[i] = Marshal.PtrToStringBSTR(idPointers[i]);
                names[i] = Marshal.PtrToStringBSTR(namePointers[i]);
                Marshal.FreeBSTR(idPointers[i]);
                Marshal.FreeBSTR(namePointers[i]);
            }
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern IntPtr get_current_effect();
        public static string GetCurrentEffect() {
            var namePointer = get_current_effect();
            var retval = Marshal.PtrToStringBSTR(namePointer);
            Marshal.FreeBSTR(namePointer);
            return retval;
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern void set_effect(string effect_id);
        public static void SetEffect(string effectId) {
            set_effect(effectId);
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern void get_HSL(string LED_id, ref float h, ref float s, ref float l);
        public static void GetHSL(string LED_id, ref float h, ref float s, ref float l) {
            get_HSL(LED_id, ref h, ref s, ref l);
        }

        [DllImport(dllPath, CharSet = CharSet.Unicode)]
        private static extern void set_HSL(string LED_id, float h, float s, float l);
        public static void SetHSL(string LED_id, float h, float s, float l) {
            set_HSL(LED_id, h, s, l);
        }

        [DllImport(dllPath)]
        private static extern void apply();
        public static void Apply() {
            apply();
        }
    }
}

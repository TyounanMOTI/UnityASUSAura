using UnityEngine;

namespace ASUSAura {
    // ASUS Aura システムのトップレベルクラス。シングルトンです。
    // とりあえずシングルスレッドアクセスのみ想定しています。
    public class Device {
        static Device instance_;
        public static Device instance {
            get {
                if (instance_ == null) {
                    instance_ = new Device();
                }
                return instance_;
            }
        }

        // ASUS Aura システムを利用できるかどうか
        public bool available {
            get {
                return Lib.IsAvailable();
            }
        }

        // デバイス名一覧。現状、デバイスを選択する機能はないもよう (2017/06/03)
        public string[] deviceNames {
            get {
                return Lib.GetDeviceNameList();
            }
        }

        // エフェクト名の一覧
        public string[] effectNames {
            get {
                string[] ids, names;
                Lib.GetEffectList(out ids, out names);
                return names;
            }
        }

        // 現在のエフェクト名
        public string currentEffectName {
            get {
                return Lib.GetCurrentEffect();
            }
        }

        // LEDの一覧
        private LED[] _LEDs;
        public LED[] LEDs {
            get {
                if (_LEDs != null) {
                    return _LEDs;
                }
                string[] ids, names;
                Lib.GetLEDList(out ids, out names);
                _LEDs = new LED[ids.Length];
                for (int i = 0; i < ids.Length; i++) {
                    _LEDs[i] = new LED(ids[i], names[i]);
                }
                return _LEDs;
            }
        }
        
        private Device() {
            Lib.SetDebugLogFunc(Debug.LogError);
            if (!Lib.IsAvailable()) {
                return;
            }

            if (!Lib.QuerySwitchState()) {
                Lib.SetSwitchState(true);
                Lib.Apply();
            }

        }

        // すべての変更を適用する
        public void Apply() {
            Lib.Apply();
        }

        // エフェクトを設定する
        public void SetEffect(string effectName) {
            string[] ids, names;
            Lib.GetEffectList(out ids, out names);
            for (int i = 0; i < ids.Length; i++) {
                if (names[i] == effectName) {
                    Lib.SetEffect(ids[i]);
                    return;
                }
            }
        }
    }
}

using UnityEngine;

namespace ASUSAura {
    public class LED {
        private string _name;
        // LEDの名前
        public string name {
            get {
                return _name;
            }
        }

        private string _id;
        // LEDのID
        public string id {
            get {
                return _id;
            }
        }

        // LEDのHSL値を読み書き。値域は 0.0f <= x <= 1.0f
        public Vector3 colorHSL {
            get {
                var retval = Vector3.zero;
                Lib.GetHSL(id, ref retval.x, ref retval.y, ref retval.z);
                return retval;
            }
            set {
                Lib.SetHSL(id, value.x, value.y, value.z);
            }
        } 

        public LED(string id, string name) {
            _id = id;
            _name = name;
        }
    }
}

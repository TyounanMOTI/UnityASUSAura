using UnityEngine;
using System;
using System.Runtime.InteropServices;
using ASUSAura;

public class LEDController : MonoBehaviour {
    LED[] LEDs;

    private void Start() {
        if (Device.instance.available) {
            LEDs = Device.instance.LEDs;
        }
    }

    private void Update() {
        if (LEDs != null) {
            foreach (var LED in LEDs) {
                LED.colorHSL = new Vector3(0, 1.0f, 0.441921f); // 赤
            }
            Device.instance.Apply();
        }
    }
}

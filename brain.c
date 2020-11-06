// ObliviousGmn // Dokuu // October 2020 // Big Brain

#include "bongo.c"

extern keymap_config_t keymap_config;
extern uint8_t is_master;

// Has RGB?
#ifdef RGBLIGHT_ENABLE
extern rgblight_config_t rgblight_config;
#endif

// Has Oled?
#ifdef OLED_DRIVER_ENABLE
static uint32_t oled_timer = 0;
#endif

int RGB_current_mode;

// Switches off Game layer when idle
void matrix_scan_user(void) {
     if (timer_elapsed32(oled_timer) > 300000 && timer_elapsed32(oled_timer) < 499999 ) {
     if (get_highest_layer(layer_state) == _GAME) {
          layer_off(_GAME);
          layer_on(_QWERTY);
       }
         return;
    }
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
    if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
      layer_on(layer3);
  }
    else {
      layer_off(layer3);
  }
}

// RGB
void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
}

// Oled Rotations
#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (is_master) {
    return OLED_ROTATION_270;
  }
    else {
    return OLED_ROTATION_180;
  }
}

// Master Oled
void render_status_main(void) {
    render_space();
    render_layer_state();
    render_space();
    render_mod_status_gui_alt(get_mods()|get_oneshot_mods());
    render_mod_status_ctrl_shift(get_mods()|get_oneshot_mods());
    render_space();
    render_logo();
    render_space();
}

// Slave Oled
void render_status_secondary(void) {
  switch (get_highest_layer(layer_state)){
    case _GAME:
        render_game_r();
      //  testing_game();
        break;
   case _WEAPON:
        render_weapon_r();
        break;
    default:
       render_anim();
 }
}

// Oled Sleeps
void oled_task_user(void) {
   if (timer_elapsed32(oled_timer) > 60000) {
      oled_off();
      return;
}

// Establishing Sides
#ifndef SPLIT_KEYBOARD
    else { oled_on(); }
#endif

    if (is_master) {
        render_status_main();
    } else {
        render_status_secondary();
    }
}
#endif

// Oled Wakes
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
#ifdef OLED_DRIVER_ENABLE
        oled_timer = timer_read32();
#endif
    // set_timelog();
}

switch (keycode) {
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case ADJUST:
        if (record->event.pressed) {
          layer_on(_ADJUST);
        } else {
          layer_off(_ADJUST);
        }
        return false;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      #ifdef RGB_MATRIX_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgb_matrix_default();
          rgb_matrix_enable();
        }
      #endif
      break;
  }
  return true;
}

#ifdef RGB_MATRIX_ENABLE

void suspend_power_down_keymap(void) {
    rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_keymap(void) {
    rgb_matrix_set_suspend_state(false);
}

#endif

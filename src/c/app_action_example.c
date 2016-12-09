#include <pebble.h>
#include "app_action_example.h"

static Window *s_window;
static TextLayer *s_txt_layer;

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}

static void prv_init(void) {
  prv_init_app_message();

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, false);
}

static void prv_init_app_message() {
  // Initialize AppMessage
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //Create a TextLayer to show the result
  s_txt_layer = text_layer_create(GRect(7, 0, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_txt_layer, GColorClear);
  text_layer_set_text_color(s_txt_layer, GColorBlack);
  text_layer_set_font(s_txt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_txt_layer, GTextAlignmentLeft);
  text_layer_set_text(s_txt_layer, "Loading Kraken data");
  layer_add_child(window_layer, text_layer_get_layer(s_txt_layer));
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_APP_READY);
  if (ready_tuple) {
    if(launch_reason() == APP_LAUNCH_USER || launch_reason() == APP_LAUNCH_QUICK_LAUNCH) {
      // Toggle the Lockitron!
      prv_lockitron_toggle_state();
    } else {
      // Application was just installed, or configured
      text_layer_set_text(s_txt_layer, "The awesome Kraken.com   Pebble App has been installed!");
    }
    return;
  }

  Tuple *balance_eth_tuple = dict_find(iter, MESSAGE_KEY_BALANCE_ETH);
	Tuple *balance_eur_tuple = dict_find(iter, MESSAGE_KEY_BALANCE_EUR);
	Tuple *current_eth_tuple = dict_find(iter, MESSAGE_KEY_CURRENT);
	Tuple *low_eth_tuple = dict_find(iter, MESSAGE_KEY_LOW);
	Tuple *high_eth_tuple = dict_find(iter, MESSAGE_KEY_HIGH);
	static char str[100];
	snprintf(str, sizeof(str), "ETH: %s\nEUR: %s\n\nLow: %s\nHigh: %s\nCurrent: %s", balance_eth_tuple->value->cstring, balance_eur_tuple->value->cstring, low_eth_tuple->value->cstring, high_eth_tuple->value->cstring, current_eth_tuple->value->cstring);
	text_layer_set_text(s_txt_layer, str);

	// Exit the application, after timeout
	prv_exit_delay();
}

static void prv_exit_delay() {
  // Get the system timeout duration
  //int timeout = preferred_result_display_duration();
  // After the timeout, exit the application
  //AppTimer *timer = app_timer_register(timeout, prv_exit_application, NULL);
}

static void prv_exit_application(void *data) {
  // App can exit to return directly to their default watchface
  //exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);

  // Exit the application by unloading the only window
  //window_stack_remove(s_window, false);
}

// Request a state change for the Lockitron (Unlock/Lock)
static void prv_lockitron_toggle_state() {
  DictionaryIterator *out;
  AppMessageResult result = app_message_outbox_begin(&out);
  if (result != APP_MSG_OK) {
    text_layer_set_text(s_txt_layer, "Outbox Failed");
  }

  result = app_message_outbox_send();
  if (result != APP_MSG_OK) {
    text_layer_set_text(s_txt_layer, "Message Failed");
  }
}

static void prv_window_unload(Window *window) {
  window_destroy(s_window);
}

static void prv_deinit(void) {
  // Before the application terminates, setup the AppGlance
  //app_glance_reload(prv_update_app_glance);
}

// Create the AppGlance displayed in the system launcher
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit, void *context) {
  // Check we haven't exceeded system limit of AppGlance's
	APP_LOG(APP_LOG_LEVEL_INFO, "In glance");
  if (limit < 1) return;

  // Retrieve the current Lockitron state from context
  //LockitronLockState *lockitron_state = context;
	char ja[50];
	strncpy(ja, context, 20);
	ja[20] = '\0';
  // Generate a friendly message for the current Lockitron state
  char str[50];
  snprintf(str, sizeof(str), "%s", ja);
  APP_LOG(APP_LOG_LEVEL_INFO, "STATE: %s", ja);

  // Create the AppGlanceSlice (no icon, no expiry)
  AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .subtitle_template_string = str
    },
    .expiration_time = APP_GLANCE_SLICE_NO_EXPIRATION
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);
  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}

// Generate a string to display the Lockitron state
const char * prv_lockitron_status_message() {
	return "ok";
}

#pragma once

struct UISettings
{
	/*explicit UISettings(float ui_scale = 1.0f)
		: ui_scale(ui_scale) {}*/

	explicit UISettings(float ui_scale = 1.0f) : ui_scale(ui_scale) {};

	float ui_scale = 1.0f;

	float item_description_label_width = 250.0f * ui_scale;
	float item_description_label_height = 25.0f * ui_scale;

	float item_description_id_position_x = 200.0f * ui_scale;
	
	float item_description_icon_width = 50.0f * ui_scale;
	float item_description_icon_height = 50.0f * ui_scale;

	float item_description_step_y = 50.0f * ui_scale;

	float item_description_components_offset_x = 10.0f * ui_scale;

	int craft_button_rows = 5;
	int craft_button_columns = 5;

	float craft_button_position_x = 660.0f * ui_scale;
	float craft_button_position_y = 0.0f * ui_scale;

	float craft_button_width = 60.0f * ui_scale;
	float craft_button_height = 60.0f * ui_scale;

	float inventory_slot_width = 50.0f * ui_scale;
	float inventory_slot_height = 50.0f * ui_scale;

	float item_name_text_scale_x = 0.6f * ui_scale;
	float item_name_text_scale_y = 0.6f * ui_scale;
	 
	float item_id_text_scale_x = 0.5f * ui_scale;
	float item_id_text_scale_y = 0.5f * ui_scale;

	float item_recipe_text_scale_x = 0.6f * ui_scale;
	float item_recipe_text_scale_y = 0.6f * ui_scale;

	float crafting_component_text_scale_x = 0.5f * ui_scale;
	float crafting_component_text_scale_y = 0.5f * ui_scale;
};
import shutil
import os


def before_all(context):
    context.screens_dir = '/quant-trade-studio/logs/e2e_screens'
    if os.path.exists(context.screens_dir):
        shutil.rmtree(context.screens_dir)
    os.makedirs(context.screens_dir)

def before_scenario(context, scenario):
    context.step_index = 0

def after_step(context, step):
    if not hasattr(context, 'webdriver'):
        return

    context.step_index += 1

    step_name = step.name.replace(" ", "_").replace("/", "_")
    scenario_name = context.scenario.name.replace(" ", "_").replace("/", "_")

    step_num = str(context.step_index).zfill(2)
    file_path = context.screens_dir + "/" + scenario_name
    # max filename length on Unix is 255 characters (including the extension)

    unit_max_filename_length = 255
    file_extension = ".png"
    max_filename_length_without_extension = unit_max_filename_length - len(file_extension)
    file_name = f"{step_num}__{step_name}"
    if len(file_name) > max_filename_length_without_extension:
        file_name = file_name[:max_filename_length_without_extension]
    file_name_with_extension = file_name + file_extension

    os.makedirs(file_path, exist_ok=True)
    context.webdriver.save_screenshot(file_path + "/" + file_name_with_extension)

def after_scenario(context, scenario):
    with open("/quant-trade-studio/logs/e2e_tests/browser_logs.txt", "w") as f:
        for entry in context.webdriver.get_log("browser"):
            f.write(f"{entry['level']} - {entry['message']}\n")

    if hasattr(context, 'market_service_mock'):
        context.market_service_mock.stop()

    if hasattr(context, 'webdriver'):
        context.webdriver.quit()

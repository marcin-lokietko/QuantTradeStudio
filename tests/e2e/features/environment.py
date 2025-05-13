import shutil
import os


def before_all(context):
    context.screens_dir = '/algo-trader/logs/e2e_screens'
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
    file_name = f"{step_num}__{step_name}.png"

    os.makedirs(file_path, exist_ok=True)
    context.webdriver.save_screenshot(file_path + "/" + file_name)

def after_scenario(context, scenario):
    with open("/algo-trader/logs/e2e_tests/browser_logs.txt", "w") as f:
        for entry in context.webdriver.get_log("browser"):
            f.write(f"{entry['level']} - {entry['message']}\n")

    if hasattr(context, 'marketServiceMock'):
        context.marketServiceMock.stop()

    if hasattr(context, 'marketServiceMockwebdriver'):
        context.webdriver.quit()

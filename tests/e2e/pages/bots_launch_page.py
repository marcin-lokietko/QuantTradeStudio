from selenium.webdriver.common.by import By
from tests.e2e.utils.waiting_webdriver import WaitingWebdriver

class BotsLaunchPage:
    def __init__(self, webdriver):
        self.waiting_webdriver = WaitingWebdriver(webdriver, timeout=15)

    def select_bot(self, bot_name):
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, f'[data-testid="bots-launch-page-bot-list-elem-{bot_name}"]').click()

    def set_execution_period(self, execution_period):
        execution_period_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-execution-period"]')
        execution_period_input.clear()
        execution_period_input.send_keys(execution_period)

    def toggle_is_executed_immediately(self):
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-is-executed-immediately-checkbox"] div').click()

    def set_quote_asset(self, quote_asset):
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-quote-asset"]').click()
        list_element_id = "bots-launch-page-quote-asset-" + quote_asset
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, f'[data-testid="{list_element_id}"]').click()

    def add_base_asset(self, base_asset):
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="bots-launch-page-add-base-asset-button"]', is_fallback_to_js_click=True)

        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-base-asset"]')
        list_element_id = "add-asset-and-number-dialog-base-asset-" + base_asset
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, f'[data-testid="{list_element_id}"]')

        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-confirm-button"]')

    def add_base_asset_share(self, base_asset, base_asset_share):
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="bots-launch-page-add-base-asset-button"]', is_fallback_to_js_click=True)

        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-base-asset"]')
        list_element_id = "add-asset-and-number-dialog-base-asset-" + base_asset
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, f'[data-testid="{list_element_id}"]')

        base_asset_share_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-asset-share"]')
        base_asset_share_input.clear()
        base_asset_share_input.send_keys(base_asset_share)

        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-confirm-button"]')

    def click_launch_bot_button(self):
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="bots-launch-page-launch-bot-button"]')

    def click_open_bot_backtest_dialog_button(self):
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="bots-launch-page-open-backtest-dialog-button"]')

    def set_short_term_moving_average_length(self, short_average_length):
        short_average_length_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-short-term-moving-average-length"]')
        short_average_length_input.clear()
        short_average_length_input.send_keys(short_average_length)

    def set_long_term_moving_average_length(self, long_average_length):
        long_average_length_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-long-term-moving-average-length"]')
        long_average_length_input.clear()
        long_average_length_input.send_keys(long_average_length)

    def set_execution_interval(self, execution_interval):
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-execution-interval"]').click()
        list_element_id = "bots-launch-page-execution-interval-" + execution_interval
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, f'[data-testid="{list_element_id}"]').click()

    def set_exit_channel_length(self, exit_channel_length):
        exit_channel_length_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-exit-channel-length"]')
        exit_channel_length_input.clear()
        exit_channel_length_input.send_keys(exit_channel_length)

    def set_entry_channel_length(self, entry_channel_length):
        entry_channel_length_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="bots-launch-page-entry-channel-length"]')
        entry_channel_length_input.clear()
        entry_channel_length_input.send_keys(entry_channel_length)

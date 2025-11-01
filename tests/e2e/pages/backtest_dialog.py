from selenium.webdriver.common.by import By
from tests.e2e.utils.waiting_webdriver import WaitingWebdriver
from selenium.webdriver.common.keys import Keys

class BacktestDialog:
    def __init__(self, webdriver):
        self.waiting_webdriver = WaitingWebdriver(webdriver, timeout=15)

    def set_fee_percent(self, fee_percent):
        fee_percent_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="backtest-dialog-transaction-fee-percent"]')
        fee_percent_input.clear()
        fee_percent_input.send_keys(fee_percent)

    def add_owned_asset(self, owned_asset, owned_asset_amount):
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="backtest-dialog-add-asset-button"]', is_fallback_to_js_click=True)

        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-base-asset"]')
        list_element_id = "add-asset-and-number-dialog-base-asset-" + owned_asset
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, f'[data-testid="{list_element_id}"]')

        base_asset_share_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-asset-share"]')
        base_asset_share_input.clear()
        base_asset_share_input.send_keys(owned_asset_amount)

        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="add-asset-and-number-dialog-confirm-button"]')

    def set_simulation_start_date(self, start_date):
        self._set_date('[data-testid="backtest-dialog-start-date"]', start_date)

    def set_simulation_start_time(self, start_time):
        start_time_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="backtest-dialog-start-time"]')
        start_time_input.clear()
        start_time_input.send_keys(start_time)

    def set_simulation_end_date(self, end_date):
        self._set_date('[data-testid="backtest-dialog-end-date"]', end_date)

    def set_simulation_end_time(self, end_time):
        end_time_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="backtest-dialog-end-time"]')
        end_time_input.clear()
        end_time_input.send_keys(end_time)

    def click_launch_backtest_button(self):
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, '[data-testid="backtest-dialog-launch-button"]')

    def get_backtest_results(self):
        # Assumes the numerical values are shown after ":"
        total_profit_if_held_text = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="backtest-results-total-profit-if-held"]').text
        total_profit_if_held = total_profit_if_held_text.split(':')[-1].strip()
        total_profit_with_trades_text = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="backtest-results-total-profit-with-trades"]').text
        total_profit_with_trades = total_profit_with_trades_text.split(':')[-1].strip()

        return {
            'total_profit_if_held': total_profit_if_held,
            'total_profit_with_trades': total_profit_with_trades
        }

    def _set_date(self, date_input_selector, date_value):
        date_input = self.waiting_webdriver.find_element(By.CSS_SELECTOR, date_input_selector)
        self.waiting_webdriver.click_when_clickable(By.CSS_SELECTOR, date_input_selector, True)

        # Material Design datepickers don't work well with clear() and send_keys() 
        date_input.send_keys(Keys.CONTROL + "a")  # Select all
        date_input.send_keys(Keys.DELETE)
        date_input.send_keys(date_value)
        date_input.send_keys(Keys.ENTER)

from selenium.webdriver.common.by import By
from tests.e2e.utils.waiting_webdriver import WaitingWebdriver

class MakeOrderDialog:
    def __init__(self, webdriver):
        self.waiting_webdriver = WaitingWebdriver(webdriver, timeout=15)

    def set_quote_asset(self, quoteAsset):
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="make-order-dialog-quote-asset"]').click()
        list_element_id = "make-order-dialog-quote-asset-" + quoteAsset
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, f'[data-testid="{list_element_id}"]').click()

    def set_order_side(self, orderSide):
        radio_button_id = ""
        if orderSide.lower() == "buy":
            radio_button_id = "order-side-radio-buy"
        elif orderSide.lower() == "sell":
            radio_button_id = "order-side-radio-sell"
        else:
            raise Exception(f"Unknown order side: {orderSide}")

        self.waiting_webdriver.find_element(By.CSS_SELECTOR, f'[data-testid="{radio_button_id}"]').click()

    def set_amount(self, amount):
        elem = self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="make-order-dialog-amount"]')
        elem.click()
        elem.clear()
        elem.send_keys(amount)

    def click_confirm_button(self):
        self.waiting_webdriver.find_element(By.CSS_SELECTOR, '[data-testid="make-order-dialog-confirm"]').click()


from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import ElementClickInterceptedException

class WaitingWebdriver:
    def __init__(self, webdriver, timeout=10):
        self.webdriver = webdriver
        self.timeout = timeout

    def find_element(self, by, value):
        wait = WebDriverWait(self.webdriver, self.timeout)
        return wait.until(EC.presence_of_element_located((by, value)))

    def click_when_clickable(self, by, value, is_fallback_to_js_click=False):
        wait = WebDriverWait(self.webdriver, self.timeout)

        # Wait for element to be in the DOM and be visible
        wait.until(EC.presence_of_element_located((by, value)))
        wait.until(EC.visibility_of_element_located((by, value)))
        element = wait.until(EC.element_to_be_clickable((by, value)))

        self.webdriver.execute_script("arguments[0].scrollIntoView(true);", element)

        # Try native click, but fallback to forced JS click if intercepted
        try:
            element.click()
        except ElementClickInterceptedException:
            if not is_fallback_to_js_click:
                raise
            print("Warning - click intercepted, using JS click instead")
            self.webdriver.execute_script("arguments[0].click();", element)

        return element
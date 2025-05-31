from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

class WaitingWebdriver:
    def __init__(self, webdriver, timeout=10):
        self.webdriver = webdriver
        self.timeout = timeout

    def find_element(self, by, value):
        wait = WebDriverWait(self.webdriver, self.timeout)
        return wait.until(EC.presence_of_element_located((by, value)))

    def click_when_clickable(self, by, value, ignore_exceptions=False):
        # element = self.webdriver.find_element(by, value)
        # self.webdriver.execute_script("arguments[0].scrollIntoView(true);", element)

        wait = WebDriverWait(self.webdriver, self.timeout)
        element = wait.until(EC.element_to_be_clickable((by, value)))

        if ignore_exceptions:
            try:
                element.click()
            except Exception:
                pass
        else:
            element.click()

        return element

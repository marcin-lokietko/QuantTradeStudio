from selenium.webdriver.common.by import By
from tests.e2e.utils.waiting_webdriver import WaitingWebdriver

class OrdersPage:
    def __init__(self, webdriver):
        self.waiting_webdriver = WaitingWebdriver(webdriver, timeout=15)

    def get_active_orders(self):
        ''' Returns [{assetPairCell, orderSideCell, priceCell, origQuantityCell, executedQuantityCell}] '''
        assets = []

        asset_table_body = self.waiting_webdriver.find_element(By.XPATH, "//tbody")
        asset_table_entries = asset_table_body.find_elements(By.TAG_NAME, "tr")

        for single_entry in asset_table_entries:
            assetPairCell = single_entry.find_element(By.CLASS_NAME, "mat-column-assetPair")
            orderSideCell = single_entry.find_element(By.CLASS_NAME, "mat-column-orderSide")
            priceCell = single_entry.find_element(By.CLASS_NAME, "mat-column-price")
            origQuantityCell = single_entry.find_element(By.CLASS_NAME, "mat-column-origQuantity")
            executedQuantityCell = single_entry.find_element(By.CLASS_NAME, "mat-column-executedQuantity")

            assets.append({
                "assetPairCell": assetPairCell,
                "orderSideCell": orderSideCell,
                "priceCell": priceCell,
                "origQuantityCell": origQuantityCell,
                "executedQuantityCell": executedQuantityCell,
            })

        return assets

    def get_active_orders_texts(self):
        ''' Returns [{assetPairText, orderSideText, priceText, origQuantityText, executedQuantityText}] '''
        orders = self.get_active_orders()
        orders_texts = []
        for single_entry in orders:
            orders_texts.append({
                "assetPairText": single_entry["assetPairCell"].text,
                "orderSideText": single_entry["orderSideCell"].text,
                "priceText": single_entry["priceCell"].text,
                "origQuantityText": single_entry["origQuantityCell"].text,
                "executedQuantityText": single_entry["executedQuantityCell"].text,
            })
        return orders_texts

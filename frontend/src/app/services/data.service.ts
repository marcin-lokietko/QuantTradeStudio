import { Injectable } from '@angular/core';
import { Balance } from '@app/account/pages/assets/assets-page.component';
import { Order } from '@app/account/pages/orders/orders-page.component';
import { environment } from '@env/environment';

export enum RequestResult {
  Success = 'Success',
  Fail = 'Fail',
}

@Injectable({
  providedIn: 'root',
})
export class DataService {
  async getOwnedAssets(): Promise<Balance[] | undefined> {
    console.log('DataService.getOwnedAssets');
    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/assets');
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET /assets response:', JSON.stringify(data));
      return data as Balance[];
    } catch (error) {
      console.error('GET /assets error:', error);
      return undefined;
    }
  }

  async getOpenOrders(): Promise<Order[] | undefined> {
    console.log('DataService.getOpenOrders');
    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/openOrders');
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET /openOrders response:', JSON.stringify(data));
      return data as Order[];
    } catch (error) {
      console.error('GET /openOrders error:', error);
      return undefined;
    }
  }

  async makeOrder(
    selectedBaseAsset: string,
    selectedQuoteAsset: string,
    orderSide: string,
    baseAssetAmount: string,
  ): Promise<RequestResult> {
    console.log(
      `DataService.makeOrder selectedBaseAsset=${selectedBaseAsset}, selectedQuoteAsset=${selectedQuoteAsset}, orderSide=${orderSide}, baseAssetAmount=${baseAssetAmount}`,
    );

    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/makeOrder', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Access-Control-Allow-Headers': '*',
        },
        body: JSON.stringify({ selectedBaseAsset, selectedQuoteAsset, orderSide, baseAssetAmount }),
      });
      if (!response.ok) {
        return RequestResult.Fail;
      }
      console.info('POST /makeOrder success:');
      return RequestResult.Success;
    } catch (error) {
      console.error('POST /makeOrder error:', error);
      return RequestResult.Fail;
    }
  }
}

import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { MatSort } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';
import { MatDialog } from '@angular/material/dialog';
import { MakeOrderDialog } from '../../../shared/components/make-order-dialog/make-order-dialog.component';
import { NotificationService } from '../../../services/notification.service';
import { NotificationSeverity } from '../../../shared/components/notification/notification-severity-enum';
import { environment } from '@env/environment';

export interface Balance {
  assetSymbol: string;
  freeQuantity: string;
  usdtValue: string;
}

@Component({
  selector: 'app-assets-page',
  standalone: false,
  templateUrl: './assets-page.component.html',
  styleUrls: ['./assets-page.component.scss']
})
export class AssetsPage {
  public balances: Balance[] = [];
  public isLoading = true;
  public displayedColumns: string[] = ['assetSymbol', 'freeQuantity', 'usdtValue'];

  dataSource = new MatTableDataSource(this.balances);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(private dialog: MatDialog, private notificationService: NotificationService, private cdr: ChangeDetectorRef) { }

  ngOnInit(): void {
    this.getData();
  }

  showMakeOrderDialog(element: any): void {
    const dialogRef = this.dialog.open(MakeOrderDialog, {
      width: '60vw',
      data: {
        initialAssetToBuy: element.assetSymbol
      }
    });

    dialogRef.afterClosed().subscribe(result => {
      console.log('Dialog closed with result:', result);
      if (result) {
        this.makeOrder(result.selectedBaseAsset, result.selectedQuoteAsset, result.orderSide, result.baseAssetAmount);
      }
    });
  }

  getData(): void {
    fetch(environment.algoTraderBackendUrlPrefix + '/assets')
      .then(response => {
        this.isLoading = false;
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then(data => {
        try {
          this.balances = data as Balance[];
          this.dataSource = new MatTableDataSource(this.balances);
          this.dataSource.sort = this.sort;
          this.cdr.markForCheck();
        } catch (error) {
          console.error("Invalid assets content:", error);
        }
        console.log('API Response:', JSON.stringify(this.balances));
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }

  makeOrder(selectedBaseAsset: string, selectedQuoteAsset: string, orderSide: string, baseAssetAmount: string): void {
    console.log(`tradeAsset selectedBaseAsset=${selectedBaseAsset}, selectedQuoteAsset=${selectedQuoteAsset}, orderSide=${orderSide}, baseAssetAmount=${baseAssetAmount}`);

    fetch(environment.algoTraderBackendUrlPrefix + '/makeOrder', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Headers': '*'
      },
      body: JSON.stringify({ selectedBaseAsset, selectedQuoteAsset, orderSide, baseAssetAmount })
    })
      .then(response => {
        if (!response.ok) {
          this.notificationService.show('Failed to open the order', 3000, NotificationSeverity.Error);
          throw new Error('makeOrder HTTP error ' + response.status);
        }
        return response.json();
      })
      .then(data => {
        console.log('makeOrder successful:', data);
        this.getData();
      })
      .catch(error => {
        console.error('makeOrder failed:', error);
      });
  }
}

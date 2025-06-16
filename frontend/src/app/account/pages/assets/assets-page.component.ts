import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { MatSort } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';
import { MatDialog } from '@angular/material/dialog';
import { MakeOrderDialog } from '@app/shared/components/make-order-dialog/make-order-dialog.component';
import { NotificationService } from '@app/services/notification.service';
import { NotificationSeverity } from '@app/shared/components/notification/notification-severity-enum';
import { Balance, DataService, RequestResult } from '@app/services/data.service';

@Component({
  selector: 'app-assets-page',
  standalone: false,
  templateUrl: './assets-page.component.html',
  styleUrls: ['./assets-page.component.scss'],
})
export class AssetsPage {
  public balances: Balance[] = [];
  public isLoading = true;
  public displayedColumns: string[] = ['assetSymbol', 'freeQuantity', 'usdtValue'];

  dataSource = new MatTableDataSource(this.balances);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(
    private dialog: MatDialog,
    private notificationService: NotificationService,
    private cdr: ChangeDetectorRef,
    private dataService: DataService,
  ) {}

  ngOnInit(): void {
    this.getOwnedAssets();
  }

  showMakeOrderDialog(element: any): void {
    const dialogRef = this.dialog.open(MakeOrderDialog, {
      width: '60vw',
      data: {
        initialAssetToBuy: element.assetSymbol,
      },
    });

    dialogRef.afterClosed().subscribe((result) => {
      console.log('Dialog closed with result:', result);
      if (result) {
        this.makeOrder(result.selectedBaseAsset, result.selectedQuoteAsset, result.orderSide, result.baseAssetAmount);
      }
    });
  }

  getOwnedAssets(): void {
    this.dataService.getOwnedAssets().then((data) => {
      this.isLoading = false;
      if (data) {
        this.balances = data;
        this.dataSource = new MatTableDataSource(this.balances);
        this.dataSource.sort = this.sort;
        this.cdr.markForCheck();
      }
    });
  }

  makeOrder(selectedBaseAsset: string, selectedQuoteAsset: string, orderSide: string, baseAssetAmount: string): void {
    this.dataService.makeOrder(selectedBaseAsset, selectedQuoteAsset, orderSide, baseAssetAmount).then((result) => {
      if (result === RequestResult.Success) {
        this.getOwnedAssets();
      } else if (result === RequestResult.Fail) {
        this.notificationService.show('Failed to open the order', 3000, NotificationSeverity.Error);
      }
    });
  }
}

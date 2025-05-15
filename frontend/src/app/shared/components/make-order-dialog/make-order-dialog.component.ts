import { ChangeDetectorRef, Component } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatDialogModule } from '@angular/material/dialog';
import { MAT_DIALOG_DATA, MatDialogRef } from '@angular/material/dialog';
import { Inject } from '@angular/core';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { MatSelectModule } from '@angular/material/select';
import { MatRadioModule } from '@angular/material/radio';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { environment } from '@env/environment';

@Component({
  selector: 'app-dialog',
  standalone: true,
  imports: [
    CommonModule,
    FormsModule,
    MatButtonModule,
    MatDialogModule,
    MatFormFieldModule,
    MatIconModule,
    MatInputModule,
    MatMenuModule,
    MatProgressSpinnerModule,
    MatRadioModule,
    MatSelectModule,
  ],
  templateUrl: './make-order-dialog.component.html',
  styleUrls: ['./make-order-dialog.component.scss'],
})
export class MakeOrderDialog {
  public baseAssetAmount = '';
  public availableBaseAssets: string[] = [];
  public selectedBaseAsset = '';
  public areAvailableBaseAssetsLoading = true;

  public availableQuoteAssets = [''];
  public selectedQuoteAsset = '';
  public orderSide = '';
  public areAvailableQuoteAssetsLoading = true;

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    public dialogRef: MatDialogRef<Component>,
    private cdr: ChangeDetectorRef,
  ) {}

  ngOnInit(): void {
    this.fetchAvailableBaseAssets();
  }

  closeWithResult() {
    this.dialogRef.close({
      selectedBaseAsset: this.selectedBaseAsset,
      selectedQuoteAsset: this.selectedQuoteAsset,
      orderSide: this.orderSide,
      baseAssetAmount: this.baseAssetAmount,
    });
  }

  cancel() {
    this.dialogRef.close(undefined);
  }

  isInputValid(): boolean {
    return this.baseAssetAmount != '' && this.selectedBaseAsset != '' && this.selectedQuoteAsset != '';
  }

  public get title(): string {
    let pairSymbol = '';
    if (this.selectedBaseAsset != '' && this.selectedQuoteAsset != '') {
      pairSymbol = this.selectedBaseAsset + '/' + this.selectedQuoteAsset;
    }

    return 'Trade ' + pairSymbol;
  }

  public get subtitle(): string {
    return 'Spot market order';
  }

  public get details(): string {
    if (!this.isInputValid()) {
      return '';
    }
    return 'You are buying ' + this.baseAssetAmount + ' ' + this.selectedBaseAsset + ' for ' + this.selectedQuoteAsset;
  }

  public get isQuoteAssetSelectionVisible(): boolean {
    return this.selectedBaseAsset != '';
  }

  public get isOrderSideSelectionVisible(): boolean {
    return this.isQuoteAssetSelectionVisible && this.selectedQuoteAsset != '';
  }

  public get isInputFieldVisible(): boolean {
    return this.isOrderSideSelectionVisible && this.orderSide != '';
  }

  public get inputFieldLabel(): string {
    return 'How much ' + this.selectedBaseAsset + ' to ' + this.orderSide.toLowerCase();
  }

  public onAssetToBuyChange(): void {
    this.selectedQuoteAsset = '';
    this.cdr.markForCheck();
    this.fetchAvailableQuoteAssets();
  }

  public fetchAvailableBaseAssets(): void {
    this.areAvailableBaseAssetsLoading = true;
    fetch(environment.algoTraderBackendUrlPrefix + '/availableBaseAssets', {
      method: 'GET',
    })
      .then((response) => {
        if (!response.ok) {
          throw new Error('availableBaseAssets HTTP error ' + response.status);
        }
        return response.json();
      })
      .then((data) => {
        this.availableBaseAssets = data.map((elem: any) => {
          return elem.assetSymbol;
        });
        this.selectedBaseAsset = this.data.initialAssetToBuy;
        this.areAvailableBaseAssetsLoading = false;
        this.fetchAvailableQuoteAssets();
        this.cdr.markForCheck();

        console.log('availableBaseAssets successful:', data);
      })
      .catch((error) => {
        console.error('availableBaseAssets failed:', error);
      });
  }

  public fetchAvailableQuoteAssets(): void {
    this.areAvailableQuoteAssetsLoading = true;
    const params = new URLSearchParams({
      baseAsset: this.selectedBaseAsset,
    });
    fetch(environment.algoTraderBackendUrlPrefix + `/availableQuoteAssets?${params.toString()}`, {
      method: 'GET',
    })
      .then((response) => {
        if (!response.ok) {
          throw new Error('availableQuoteAssets HTTP error ' + response.status);
        }
        return response.json();
      })
      .then((data) => {
        this.availableQuoteAssets = data.map((elem: any) => {
          return elem.quoteAsset;
        });
        this.areAvailableQuoteAssetsLoading = false;
        this.cdr.markForCheck();
        console.log('availableQuoteAssets successful:', data);
      })
      .catch((error) => {
        console.error('availableQuoteAssets failed:', error);
      });
  }
}

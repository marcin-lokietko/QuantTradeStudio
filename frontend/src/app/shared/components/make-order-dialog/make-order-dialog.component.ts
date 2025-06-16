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
import { AvailableQuoteAsset, DataService } from '@app/services/data.service';

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
  public availableBaseAssets = [''];
  public selectedBaseAsset = '';
  public areAvailableBaseAssetsLoading = true;

  public availableQuoteAssets = [''];
  public selectedQuoteAsset = '';
  public orderSide = '';
  public areAvailableQuoteAssetsLoading = true;

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    private dataService: DataService,
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
    this.dataService.getAvailableBaseAssets().then((data) => {
      if (data) {
        this.areAvailableBaseAssetsLoading = false;

        this.availableBaseAssets = data;
        this.selectedBaseAsset = this.data.initialAssetToBuy;
        this.fetchAvailableQuoteAssets();
        this.cdr.markForCheck();
      }
    });
  }

  public fetchAvailableQuoteAssets(): void {
    this.areAvailableQuoteAssetsLoading = true;
    this.dataService.getAvailableQuoteAssets(this.selectedBaseAsset).then((data) => {
      if (data) {
        this.areAvailableQuoteAssetsLoading = false;
        this.availableQuoteAssets = data.map((elem: AvailableQuoteAsset) => {
          return elem.quoteAsset;
        });
        this.cdr.markForCheck();
      }
    });
  }
}

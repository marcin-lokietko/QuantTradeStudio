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

@Component({
  selector: 'app-dialog',
  standalone: true,
  imports: [
    CommonModule,
    MatIconModule,
    MatButtonModule,
    MatMenuModule,
    MatDialogModule,
    MatInputModule,
    MatFormFieldModule,
    MatSelectModule,
    FormsModule],
  templateUrl: './make-order-dialog.component.html',
  styleUrls: ['./make-order-dialog.component.scss']
})
export class MakeOrderDialog {
  public title = 'Make order';

  public amountToBuy = '';
  public amountToSell = '';
  public availableAssetsToBuy = [ '', 'ETH', 'BTC'];
  public selectedAssetToBuy = '';

  public availableAssetsToSell = [ '', 'ETH', 'BTC', 'USDT'];
  public selectedAssetToSell = '';

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    public dialogRef: MatDialogRef<Component>,
    private cdr: ChangeDetectorRef
  ) {}

  ngOnInit(): void {
    this.selectedAssetToBuy = this.data.initialAssetToBuy;
    this.cdr.markForCheck();
    this.fetchAvailableAssetsToSell();
  }

  closeWithResult() {
    this.dialogRef.close({
      assetToBuy: this.selectedAssetToBuy,
      assetToSell: this.selectedAssetToSell,
      amountToBuy: this.amountToBuy});
  }

  cancel() {
    this.dialogRef.close(undefined);
  }

  isInputValid() : boolean {
    return this.amountToBuy != '' && this.selectedAssetToBuy != '' && this.selectedAssetToSell != '';
  }

  public get details(): string {
    if (!this.isInputValid()) {
      return '';
    }
    return "You are buying " + this.amountToBuy + " " + this.selectedAssetToBuy +
      " for " + this.amountToSell + " " + this.selectedAssetToSell;
  }

  public get isAssetToSellSelectionVisible(): boolean {
    return this.selectedAssetToBuy != '';
  }

  public get isInputFieldVisible(): boolean {
    return this.selectedAssetToBuy != '' && this.selectedAssetToSell != '';
  }

  public onAssetToBuyChange(): void {
    this.selectedAssetToSell = '';
    this.cdr.markForCheck();
    this.fetchAvailableAssetsToSell();
  }

  public fetchAvailableAssetsToSell(): void {
    console.log('fetchAvailableAssetsToSell');
  }
}


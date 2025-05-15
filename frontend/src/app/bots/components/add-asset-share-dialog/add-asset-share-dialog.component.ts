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
  templateUrl: './add-asset-share-dialog.component.html',
  styleUrls: ['./add-asset-share-dialog.component.scss'],
})
export class AddAssetShareDialog {
  public title = '';
  public inputFieldLabel = '';
  public availableAssets: string[] = [];
  public selectedAsset = '';
  public assetShare: number | undefined = undefined;

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    public dialogRef: MatDialogRef<Component>,
    private cdr: ChangeDetectorRef,
  ) {}

  ngOnInit(): void {
    this.title = this.data.title;
    this.availableAssets = this.data.availableAssets;
    this.inputFieldLabel = this.data.inputFieldLabel;
  }

  closeWithResult() {
    this.dialogRef.close({
      assetSymbol: this.selectedAsset,
      assetShare: this.assetShare,
    });
  }

  cancel() {
    this.dialogRef.close(undefined);
  }
}

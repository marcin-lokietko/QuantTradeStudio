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
  templateUrl: './add-asset-and-number-dialog.component.html',
  styleUrls: ['./add-asset-and-number-dialog.component.scss'],
})
export class AddAssetAndNumberDialog {
  public title = '';
  public inputFieldLabel = '';
  public assets: string[] = [];
  public selectedAsset = '';

  public showNumberInput = false;
  public selectedNumber: number | undefined = undefined;

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    public dialogRef: MatDialogRef<Component>,
    private cdr: ChangeDetectorRef,
  ) {}

  ngOnInit(): void {
    this.title = this.data.title;
    this.assets = this.data.assets;
    this.inputFieldLabel = this.data.inputFieldLabel;
    this.showNumberInput = this.data.showNumberInput ?? true;
  }

  closeWithResult() {
    this.dialogRef.close({
      assetSymbol: this.selectedAsset,
      number: this.selectedNumber,
    });
  }

  cancel() {
    this.dialogRef.close(undefined);
  }
}

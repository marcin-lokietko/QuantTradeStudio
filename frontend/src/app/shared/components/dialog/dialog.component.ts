import { Component, Input, ViewChild } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatDialogModule } from '@angular/material/dialog';
import { MAT_DIALOG_DATA, MatDialogRef } from '@angular/material/dialog';
import { Inject } from '@angular/core';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-dialog',
  standalone: true,
  imports: [CommonModule, MatTableModule, MatSortModule, MatIconModule, MatButtonModule, MatMenuModule, MatDialogModule, MatInputModule, MatFormFieldModule, FormsModule],
  templateUrl: './dialog.component.html',
  styleUrls: ['./dialog.component.scss']
})
export class Dialog {
  public title = '';
  public details = '';
  public isInputFieldVisible = false;
  public inputFieldLabel = '';
  public inputFieldPlaceholder = '';

  public inputValue = '';

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    public dialogRef: MatDialogRef<Component>
  ) {}

  ngOnInit(): void {
    this.title = this.data.title;
    this.details = this.data.details;
    this.isInputFieldVisible = this.data.isInputFieldVisible;
    this.inputFieldLabel = this.data.inputFieldLabel;
    this.inputFieldPlaceholder = this.data.inputFieldPlaceholder;
  }

  closeWithResult() {
    this.dialogRef.close({enteredInputValue: this.inputValue});
  }

  cancel() {
    this.dialogRef.close(undefined);
  }
}


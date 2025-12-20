import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialogModule, MatDialogRef } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatMenuModule } from '@angular/material/menu';
import { MatRadioModule } from '@angular/material/radio';
import { MatSelectModule } from '@angular/material/select';
import { ChangeDetectorRef } from '@angular/core';
import { AddAssetAndNumberDialog } from './add-asset-and-number-dialog.component';

describe('AddAssetAndNumberDialog', () => {
  let component: AddAssetAndNumberDialog;
  let fixture: ComponentFixture<AddAssetAndNumberDialog>;

  let mockDialogRef: jasmine.SpyObj<MatDialogRef<AddAssetAndNumberDialog>>;

  const mockDialogData = {
    title: 'Add Asset',
    assets: ['BTC', 'ETH'],
    inputFieldLabel: 'Share (%)',
  };

  beforeEach(async () => {
    mockDialogRef = jasmine.createSpyObj('MatDialogRef', ['close']);

    await TestBed.configureTestingModule({
      declarations: [],
      imports: [
        AddAssetAndNumberDialog,
        MatButtonModule,
        MatDialogModule,
        MatFormFieldModule,
        MatIconModule,
        MatInputModule,
        MatMenuModule,
        MatProgressSpinnerModule,
        MatRadioModule,
        MatSelectModule,
        MatSortModule,
        MatTableModule,
      ],
      providers: [
        { provide: MAT_DIALOG_DATA, useValue: mockDialogData },
        { provide: MatDialogRef, useValue: mockDialogRef },
        { provide: ChangeDetectorRef, useValue: { markForCheck: () => {} } },
      ],
    }).compileComponents();

    fixture = TestBed.createComponent(AddAssetAndNumberDialog);
    component = fixture.componentInstance;
  });

  it('should create the component', async () => {
    expect(component).toBeTruthy();
  });

  it('should initialize fields from dialog data on ngOnInit', () => {
    component.ngOnInit();
    expect(component.title).toBe(mockDialogData.title);
    expect(component.assets).toEqual(mockDialogData.assets);
    expect(component.inputFieldLabel).toBe(mockDialogData.inputFieldLabel);
  });

  it('should call dialogRef.close with result on closeWithResult()', () => {
    component.selectedAsset = 'BTC';
    component.selectedNumber = 60;
    component.closeWithResult();
    expect(mockDialogRef.close).toHaveBeenCalledWith({
      assetSymbol: 'BTC',
      number: 60,
    });
  });

  it('should call dialogRef.close with undefined on cancel()', () => {
    component.cancel();
    expect(mockDialogRef.close).toHaveBeenCalledWith(undefined);
  });
});

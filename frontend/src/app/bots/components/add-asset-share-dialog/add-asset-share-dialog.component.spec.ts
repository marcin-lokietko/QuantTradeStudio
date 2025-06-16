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
import { AddAssetShareDialog } from './add-asset-share-dialog.component';

describe('AddAssetShareDialog', () => {
  let component: AddAssetShareDialog;
  let fixture: ComponentFixture<AddAssetShareDialog>;

  let mockDialogRef: jasmine.SpyObj<MatDialogRef<AddAssetShareDialog>>;

  const mockDialogData = {
    title: 'Add Asset',
    availableAssets: ['BTC', 'ETH'],
    inputFieldLabel: 'Share (%)',
  };

  beforeEach(async () => {
    mockDialogRef = jasmine.createSpyObj('MatDialogRef', ['close']);

    await TestBed.configureTestingModule({
      declarations: [],
      imports: [
        AddAssetShareDialog,
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

    fixture = TestBed.createComponent(AddAssetShareDialog);
    component = fixture.componentInstance;
  });

  it('should create the component', async () => {
    expect(component).toBeTruthy();
  });

  it('should initialize fields from dialog data on ngOnInit', () => {
    component.ngOnInit();
    expect(component.title).toBe(mockDialogData.title);
    expect(component.availableAssets).toEqual(mockDialogData.availableAssets);
    expect(component.inputFieldLabel).toBe(mockDialogData.inputFieldLabel);
  });

  it('should call dialogRef.close with result on closeWithResult()', () => {
    component.selectedAsset = 'BTC';
    component.assetShare = 60;
    component.closeWithResult();
    expect(mockDialogRef.close).toHaveBeenCalledWith({
      assetSymbol: 'BTC',
      assetShare: 60,
    });
  });

  it('should call dialogRef.close with undefined on cancel()', () => {
    component.cancel();
    expect(mockDialogRef.close).toHaveBeenCalledWith(undefined);
  });
});

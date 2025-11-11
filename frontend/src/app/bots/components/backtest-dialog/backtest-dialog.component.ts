import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatDialog, MatDialogModule } from '@angular/material/dialog';
import { MAT_DIALOG_DATA, MatDialogRef } from '@angular/material/dialog';
import { Inject } from '@angular/core';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { MatSelectModule } from '@angular/material/select';
import { MatRadioModule } from '@angular/material/radio';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatDatepickerModule } from '@angular/material/datepicker';
import { MatNativeDateModule } from '@angular/material/core';
import { BacktestResults, DataService, RequestResult } from '@app/services/data.service';
import { NotificationSeverity } from '@app/shared/components/notification/notification-severity-enum';
import { NotificationService } from '@app/services/notification.service';
import { MatTableDataSource } from '@angular/material/table';
import { MatSort } from '@angular/material/sort';
import { MatTableModule } from '@angular/material/table';
import { AddAssetAndNumberDialog } from '../add-asset-and-number-dialog/add-asset-and-number-dialog.component';

interface AssetConfig {
  assetSymbol: string;
  freeQuantity: string;
}

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
    MatDatepickerModule,
    MatNativeDateModule,
    MatTableModule,
  ],
  templateUrl: './backtest-dialog.component.html',
  styleUrls: ['./backtest-dialog.component.scss'],
})
export class BacktestDialog {
  public title = '';

  public transactionFeePercent = '';
  public readonly transactionFeePercentInputFieldLabel = 'Transaction fee percent';

  public botParams: any;
  public initialOwnedAssets: AssetConfig[] = [];

  public simulationStartDate: Date = new Date();
  public simulationStartTime: string = '00:00';
  public simulationStartUnixTime = 0;

  public simulationEndDate: Date = new Date();
  public simulationEndTime: string = '00:00';
  public simulationEndUnixTime = 0;

  public readonly displayedInitialOwnedAssetsTableColumns: string[] = ['assetSymbol', 'freeQuantity'];

  public availableInitialAssets: string[] = [];

  public backtestResults: BacktestResults | undefined;

  dataSource = new MatTableDataSource(this.initialOwnedAssets);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(
    @Inject(MAT_DIALOG_DATA) public data: any,
    public dialogRef: MatDialogRef<Component>,
    private dialog: MatDialog,
    private dataService: DataService,
    private notificationService: NotificationService,
    private cdr: ChangeDetectorRef,
  ) {}

  ngOnInit(): void {
    this.botParams = this.data.botParams;
    this.availableInitialAssets = this.data.availableInitialAssets;
    this.title = `Backtest bot: ${this.botParams.botName}`;

    // TODO support klines paging
    // const oneMonthAgo = new Date();
    // oneMonthAgo.setMonth(oneMonthAgo.getMonth() - 1);
    // this.simulationStartDate = oneMonthAgo;
    // this.simulationStartTime = '00:00';

    const yesterday = new Date();
    yesterday.setDate(yesterday.getDate() - 1);
    this.simulationEndDate = yesterday;
    this.simulationEndTime = '23:59';

    this.simulationStartDate = yesterday;
    this.simulationStartTime = '20:00';

    this.updateUnixTimestamps();
  }

  public updateUnixTimestamps(): void {
    this.simulationStartUnixTime = this.calculateUnixTime(this.simulationStartDate, this.simulationStartTime);
    this.simulationEndUnixTime = this.calculateUnixTime(this.simulationEndDate, this.simulationEndTime);
  }

  public launchBacktest() {
    this.backtestResults = undefined;
    this.dataService.testBot(this.botParams, this.getBacktestConfig()).then((result) => {
      if (!result) {
        this.notificationService.show('Failed to launch the backtest', 3000, NotificationSeverity.Error);
        return;
      }
      this.backtestResults = result;
      console.log('Backtest results:', this.backtestResults);
    });
  }

  public closeWithResult() {
    this.dialogRef.close(undefined);
  }

  public cancel() {
    this.dialogRef.close(undefined);
  }

  public get isConfigValid(): boolean {
    return (
      this.transactionFeePercent !== '' &&
      !isNaN(Number(this.transactionFeePercent)) &&
      Number(this.transactionFeePercent) >= 0 &&
      this.simulationStartUnixTime < this.simulationEndUnixTime &&
      this.initialOwnedAssets.length > 0
    );
  }

  public deleteSelectedAssetConfig(elementToRemove: AssetConfig) {
    this.initialOwnedAssets = this.initialOwnedAssets.filter((elem: AssetConfig) => {
      return elem != elementToRemove;
    });
    this.updateAssetConfigTable();
  }

  public updateAssetConfigTable() {
    this.dataSource = new MatTableDataSource(this.initialOwnedAssets);
    this.dataSource.sort = this.sort;
    this.cdr.markForCheck();
  }

  public addAsset(): void {
    const dialogRef = this.dialog.open(AddAssetAndNumberDialog, {
      width: '60vw',
      data: {
        title: 'Add asset free quantity',
        inputFieldLabel: 'Asset free quantity',
        assets: this.availableInitialAssets.filter((elem: string) => {
          return (
            this.initialOwnedAssets.findIndex((config) => {
              return config.assetSymbol === elem;
            }) === -1
          );
        }),
      },
    });

    dialogRef.afterClosed().subscribe((result) => {
      console.log('Dialog closed with result:', result);
      if (result) {
        this.initialOwnedAssets.push({ assetSymbol: result.assetSymbol, freeQuantity: result.assetShare });
        this.updateAssetConfigTable();
      }
    });

    this.updateAssetConfigTable();
  }

  private calculateUnixTime(date: Date, time: string): number {
    //Note that Unix time is always UTC-based. The Date object in JS stores dates in local time zone,
    // but getTime() returns the time as milliseconds since the Unix Epoch (January 1, 1970, 00:00:00 UTC).
    const [hours, minutes] = time.split(':').map(Number);
    const combinedDate = new Date(date);
    combinedDate.setHours(hours, minutes, 0, 0);
    return combinedDate.getTime();
  }

  private getBacktestConfig() {
    return {
      transactionFeePercent: Number(this.transactionFeePercent),
      initialOwnedAssets: this.initialOwnedAssets,
      simulationStart: this.simulationStartUnixTime,
      simulationEnd: this.simulationEndUnixTime,
    };
  }
}
